/*
 * IXmlReader implementation
 *
 * Copyright 2010 Nikolay Sivov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#define COBJMACROS

#include <stdarg.h>
#include "windef.h"
#include "winbase.h"
#include "initguid.h"
#include "objbase.h"
#include "xmllite.h"
#include "xmllite_private.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(xmllite);

/* not defined in public headers */
DEFINE_GUID(IID_IXmlReaderInput, 0x0b3ccc9b, 0x9214, 0x428b, 0xa2, 0xae, 0xef, 0x3a, 0xa8, 0x71, 0xaf, 0xda);

typedef enum
{
    XmlEncoding_UTF16,
    XmlEncoding_UTF8,
    XmlEncoding_Unknown
} xml_encoding;

static const WCHAR utf16W[] = {'U','T','F','-','1','6',0};
static const WCHAR utf8W[] = {'U','T','F','-','8',0};

struct xml_encoding_data
{
    const WCHAR *encoding;
    xml_encoding enc;
    UINT cp;
};

static const struct xml_encoding_data xml_encoding_map[] = {
    { utf16W, XmlEncoding_UTF16, ~0 },
    { utf8W,  XmlEncoding_UTF8,  CP_UTF8 }
};

typedef struct
{
    char *data;
    unsigned int allocated;
    unsigned int written;
} encoded_buffer;

typedef struct input_buffer input_buffer;

typedef struct _xmlreaderinput
{
    IXmlReaderInput IXmlReaderInput_iface;
    LONG ref;
    /* reference passed on IXmlReaderInput creation, is kept when input is created */
    IUnknown *input;
    IMalloc *imalloc;
    /* stream reference set after SetInput() call from reader,
       stored as sequential stream, cause currently
       optimizations possible with IStream aren't implemented */
    ISequentialStream *stream;
    input_buffer *buffer;
} xmlreaderinput;

typedef struct _xmlreader
{
    IXmlReader IXmlReader_iface;
    LONG ref;
    xmlreaderinput *input;
    IMalloc *imalloc;
    XmlReadState state;
    DtdProcessing dtdmode;
    UINT line, pos;           /* reader position in XML stream */
} xmlreader;

struct input_buffer
{
    encoded_buffer utf16;
    encoded_buffer encoded;
    UINT code_page;
    xmlreaderinput *input;
};

static inline xmlreader *impl_from_IXmlReader(IXmlReader *iface)
{
    return CONTAINING_RECORD(iface, xmlreader, IXmlReader_iface);
}

static inline xmlreaderinput *impl_from_IXmlReaderInput(IXmlReaderInput *iface)
{
    return CONTAINING_RECORD(iface, xmlreaderinput, IXmlReaderInput_iface);
}

static inline void *m_alloc(IMalloc *imalloc, size_t len)
{
    if (imalloc)
        return IMalloc_Alloc(imalloc, len);
    else
        return heap_alloc(len);
}

static inline void m_free(IMalloc *imalloc, void *mem)
{
    if (imalloc)
        IMalloc_Free(imalloc, mem);
    else
        heap_free(mem);
}

/* reader memory allocation functions */
static inline void *reader_alloc(xmlreader *reader, size_t len)
{
    return m_alloc(reader->imalloc, len);
}

static inline void reader_free(xmlreader *reader, void *mem)
{
    return m_free(reader->imalloc, mem);
}

/* reader input memory allocation functions */
static inline void *readerinput_alloc(xmlreaderinput *input, size_t len)
{
    return m_alloc(input->imalloc, len);
}

static inline void readerinput_free(xmlreaderinput *input, void *mem)
{
    return m_free(input->imalloc, mem);
}

static HRESULT init_encoded_buffer(xmlreaderinput *input, encoded_buffer *buffer)
{
    const int initial_len = 0x2000;
    buffer->data = readerinput_alloc(input, initial_len);
    if (!buffer->data) return E_OUTOFMEMORY;

    memset(buffer->data, 0, 4);
    buffer->allocated = initial_len;
    buffer->written = 0;

    return S_OK;
}

static void free_encoded_buffer(xmlreaderinput *input, encoded_buffer *buffer)
{
    readerinput_free(input, buffer->data);
}

static HRESULT get_code_page(xml_encoding encoding, UINT *cp)
{
    const struct xml_encoding_data *data;

    if (encoding == XmlEncoding_Unknown)
    {
        FIXME("unsupported encoding %d\n", encoding);
        return E_NOTIMPL;
    }

    data = &xml_encoding_map[encoding];
    *cp = data->cp;

    return S_OK;
}

static HRESULT alloc_input_buffer(xmlreaderinput *input, xml_encoding encoding)
{
    input_buffer *buffer;
    HRESULT hr;

    input->buffer = NULL;

    buffer = readerinput_alloc(input, sizeof(*buffer));
    if (!buffer) return E_OUTOFMEMORY;

    buffer->input = input;
    hr = get_code_page(encoding, &buffer->code_page);
    if (hr != S_OK) {
        readerinput_free(input, buffer);
        return hr;
    }

    hr = init_encoded_buffer(input, &buffer->utf16);
    if (hr != S_OK) {
        readerinput_free(input, buffer);
        return hr;
    }

    if (encoding != XmlEncoding_UTF16) {
        hr = init_encoded_buffer(input, &buffer->encoded);
        if (hr != S_OK) {
            free_encoded_buffer(input, &buffer->utf16);
            readerinput_free(input, buffer);
            return hr;
        }
    }
    else
        memset(&buffer->encoded, 0, sizeof(buffer->encoded));

    input->buffer = buffer;
    return S_OK;
}

static void free_input_buffer(input_buffer *buffer)
{
    free_encoded_buffer(buffer->input, &buffer->encoded);
    free_encoded_buffer(buffer->input, &buffer->utf16);
    readerinput_free(buffer->input, buffer);
}

static void xmlreaderinput_release_stream(xmlreaderinput *readerinput)
{
    if (readerinput->stream) {
        ISequentialStream_Release(readerinput->stream);
        readerinput->stream = NULL;
    }
}

/* Queries already stored interface for IStream/ISequentialStream.
   Interface supplied on creation will be overwritten */
static HRESULT xmlreaderinput_query_for_stream(xmlreaderinput *readerinput)
{
    HRESULT hr;

    xmlreaderinput_release_stream(readerinput);
    hr = IUnknown_QueryInterface(readerinput->input, &IID_IStream, (void**)&readerinput->stream);
    if (hr != S_OK)
        hr = IUnknown_QueryInterface(readerinput->input, &IID_ISequentialStream, (void**)&readerinput->stream);

    return hr;
}

static HRESULT WINAPI xmlreader_QueryInterface(IXmlReader *iface, REFIID riid, void** ppvObject)
{
    xmlreader *This = impl_from_IXmlReader(iface);

    TRACE("%p %s %p\n", This, debugstr_guid(riid), ppvObject);

    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IXmlReader))
    {
        *ppvObject = iface;
    }
    else
    {
        FIXME("interface %s not implemented\n", debugstr_guid(riid));
        return E_NOINTERFACE;
    }

    IXmlReader_AddRef(iface);

    return S_OK;
}

static ULONG WINAPI xmlreader_AddRef(IXmlReader *iface)
{
    xmlreader *This = impl_from_IXmlReader(iface);
    ULONG ref = InterlockedIncrement(&This->ref);
    TRACE("(%p)->(%d)\n", This, ref);
    return ref;
}

static ULONG WINAPI xmlreader_Release(IXmlReader *iface)
{
    xmlreader *This = impl_from_IXmlReader(iface);
    LONG ref = InterlockedDecrement(&This->ref);

    TRACE("(%p)->(%d)\n", This, ref);

    if (ref == 0)
    {
        IMalloc *imalloc = This->imalloc;
        if (This->input) IUnknown_Release(&This->input->IXmlReaderInput_iface);
        reader_free(This, This);
        if (imalloc) IMalloc_Release(imalloc);
    }

    return ref;
}

static HRESULT WINAPI xmlreader_SetInput(IXmlReader* iface, IUnknown *input)
{
    xmlreader *This = impl_from_IXmlReader(iface);
    HRESULT hr;

    TRACE("(%p %p)\n", This, input);

    if (This->input)
    {
        xmlreaderinput_release_stream(This->input);
        IUnknown_Release(&This->input->IXmlReaderInput_iface);
        This->input = NULL;
    }

    This->line = This->pos = 0;

    /* just reset current input */
    if (!input)
    {
        This->state = XmlReadState_Initial;
        return S_OK;
    }

    /* now try IXmlReaderInput, ISequentialStream, IStream */
    hr = IUnknown_QueryInterface(input, &IID_IXmlReaderInput, (void**)&This->input);
    if (hr != S_OK)
    {
        IXmlReaderInput *readerinput;

        /* create IXmlReaderInput basing on supplied interface */
        hr = CreateXmlReaderInputWithEncodingName(input,
                                         NULL, NULL, FALSE, NULL, &readerinput);
        if (hr != S_OK) return hr;
        This->input = impl_from_IXmlReaderInput(readerinput);
    }

    /* set stream for supplied IXmlReaderInput */
    hr = xmlreaderinput_query_for_stream(This->input);
    if (hr == S_OK)
        This->state = XmlReadState_Initial;

    return hr;
}

static HRESULT WINAPI xmlreader_GetProperty(IXmlReader* iface, UINT property, LONG_PTR *value)
{
    xmlreader *This = impl_from_IXmlReader(iface);

    TRACE("(%p %u %p)\n", This, property, value);

    if (!value) return E_INVALIDARG;

    switch (property)
    {
        case XmlReaderProperty_DtdProcessing:
            *value = This->dtdmode;
            break;
        case XmlReaderProperty_ReadState:
            *value = This->state;
            break;
        default:
            FIXME("Unimplemented property (%u)\n", property);
            return E_NOTIMPL;
    }

    return S_OK;
}

static HRESULT WINAPI xmlreader_SetProperty(IXmlReader* iface, UINT property, LONG_PTR value)
{
    xmlreader *This = impl_from_IXmlReader(iface);

    TRACE("(%p %u %lu)\n", iface, property, value);

    switch (property)
    {
        case XmlReaderProperty_DtdProcessing:
            if (value < 0 || value > _DtdProcessing_Last) return E_INVALIDARG;
            This->dtdmode = value;
            break;
        default:
            FIXME("Unimplemented property (%u)\n", property);
            return E_NOTIMPL;
    }

    return S_OK;
}

static HRESULT WINAPI xmlreader_Read(IXmlReader* iface, XmlNodeType *node_type)
{
    FIXME("(%p %p): stub\n", iface, node_type);
    return E_NOTIMPL;
}

static HRESULT WINAPI xmlreader_GetNodeType(IXmlReader* iface, XmlNodeType *node_type)
{
    FIXME("(%p %p): stub\n", iface, node_type);
    return E_NOTIMPL;
}

static HRESULT WINAPI xmlreader_MoveToFirstAttribute(IXmlReader* iface)
{
    FIXME("(%p): stub\n", iface);
    return E_NOTIMPL;
}

static HRESULT WINAPI xmlreader_MoveToNextAttribute(IXmlReader* iface)
{
    FIXME("(%p): stub\n", iface);
    return E_NOTIMPL;
}

static HRESULT WINAPI xmlreader_MoveToAttributeByName(IXmlReader* iface,
                                                      LPCWSTR local_name,
                                                      LPCWSTR namespaceUri)
{
    FIXME("(%p %p %p): stub\n", iface, local_name, namespaceUri);
    return E_NOTIMPL;
}

static HRESULT WINAPI xmlreader_MoveToElement(IXmlReader* iface)
{
    FIXME("(%p): stub\n", iface);
    return E_NOTIMPL;
}

static HRESULT WINAPI xmlreader_GetQualifiedName(IXmlReader* iface, LPCWSTR *qualifiedName,
                                                 UINT *qualifiedName_length)
{
    FIXME("(%p %p %p): stub\n", iface, qualifiedName, qualifiedName_length);
    return E_NOTIMPL;
}

static HRESULT WINAPI xmlreader_GetNamespaceUri(IXmlReader* iface,
                                                LPCWSTR *namespaceUri,
                                                UINT *namespaceUri_length)
{
    FIXME("(%p %p %p): stub\n", iface, namespaceUri, namespaceUri_length);
    return E_NOTIMPL;
}

static HRESULT WINAPI xmlreader_GetLocalName(IXmlReader* iface,
                                             LPCWSTR *local_name,
                                             UINT *local_name_length)
{
    FIXME("(%p %p %p): stub\n", iface, local_name, local_name_length);
    return E_NOTIMPL;
}

static HRESULT WINAPI xmlreader_GetPrefix(IXmlReader* iface,
                                          LPCWSTR *prefix,
                                          UINT *prefix_length)
{
    FIXME("(%p %p %p): stub\n", iface, prefix, prefix_length);
    return E_NOTIMPL;
}

static HRESULT WINAPI xmlreader_GetValue(IXmlReader* iface,
                                         LPCWSTR *value,
                                         UINT *value_length)
{
    FIXME("(%p %p %p): stub\n", iface, value, value_length);
    return E_NOTIMPL;
}

static HRESULT WINAPI xmlreader_ReadValueChunk(IXmlReader* iface,
                                               WCHAR *buffer,
                                               UINT   chunk_size,
                                               UINT  *read)
{
    FIXME("(%p %p %u %p): stub\n", iface, buffer, chunk_size, read);
    return E_NOTIMPL;
}

static HRESULT WINAPI xmlreader_GetBaseUri(IXmlReader* iface,
                                           LPCWSTR *baseUri,
                                           UINT *baseUri_length)
{
    FIXME("(%p %p %p): stub\n", iface, baseUri, baseUri_length);
    return E_NOTIMPL;
}

static BOOL WINAPI xmlreader_IsDefault(IXmlReader* iface)
{
    FIXME("(%p): stub\n", iface);
    return E_NOTIMPL;
}

static BOOL WINAPI xmlreader_IsEmptyElement(IXmlReader* iface)
{
    FIXME("(%p): stub\n", iface);
    return E_NOTIMPL;
}

static HRESULT WINAPI xmlreader_GetLineNumber(IXmlReader* iface, UINT *lineNumber)
{
    xmlreader *This = impl_from_IXmlReader(iface);

    TRACE("(%p %p)\n", This, lineNumber);

    if (!lineNumber) return E_INVALIDARG;

    *lineNumber = This->line;

    return S_OK;
}

static HRESULT WINAPI xmlreader_GetLinePosition(IXmlReader* iface, UINT *linePosition)
{
    xmlreader *This = impl_from_IXmlReader(iface);

    TRACE("(%p %p)\n", This, linePosition);

    if (!linePosition) return E_INVALIDARG;

    *linePosition = This->pos;

    return S_OK;
}

static HRESULT WINAPI xmlreader_GetAttributeCount(IXmlReader* iface, UINT *attributeCount)
{
    FIXME("(%p %p): stub\n", iface, attributeCount);
    return E_NOTIMPL;
}

static HRESULT WINAPI xmlreader_GetDepth(IXmlReader* iface, UINT *depth)
{
    FIXME("(%p %p): stub\n", iface, depth);
    return E_NOTIMPL;
}

static BOOL WINAPI xmlreader_IsEOF(IXmlReader* iface)
{
    FIXME("(%p): stub\n", iface);
    return E_NOTIMPL;
}

static const struct IXmlReaderVtbl xmlreader_vtbl =
{
    xmlreader_QueryInterface,
    xmlreader_AddRef,
    xmlreader_Release,
    xmlreader_SetInput,
    xmlreader_GetProperty,
    xmlreader_SetProperty,
    xmlreader_Read,
    xmlreader_GetNodeType,
    xmlreader_MoveToFirstAttribute,
    xmlreader_MoveToNextAttribute,
    xmlreader_MoveToAttributeByName,
    xmlreader_MoveToElement,
    xmlreader_GetQualifiedName,
    xmlreader_GetNamespaceUri,
    xmlreader_GetLocalName,
    xmlreader_GetPrefix,
    xmlreader_GetValue,
    xmlreader_ReadValueChunk,
    xmlreader_GetBaseUri,
    xmlreader_IsDefault,
    xmlreader_IsEmptyElement,
    xmlreader_GetLineNumber,
    xmlreader_GetLinePosition,
    xmlreader_GetAttributeCount,
    xmlreader_GetDepth,
    xmlreader_IsEOF
};

/** IXmlReaderInput **/
static HRESULT WINAPI xmlreaderinput_QueryInterface(IXmlReaderInput *iface, REFIID riid, void** ppvObject)
{
    xmlreaderinput *This = impl_from_IXmlReaderInput(iface);

    TRACE("%p %s %p\n", This, debugstr_guid(riid), ppvObject);

    if (IsEqualGUID(riid, &IID_IXmlReaderInput) ||
        IsEqualGUID(riid, &IID_IUnknown))
    {
        *ppvObject = iface;
    }
    else
    {
        WARN("interface %s not implemented\n", debugstr_guid(riid));
        return E_NOINTERFACE;
    }

    IUnknown_AddRef(iface);

    return S_OK;
}

static ULONG WINAPI xmlreaderinput_AddRef(IXmlReaderInput *iface)
{
    xmlreaderinput *This = impl_from_IXmlReaderInput(iface);
    ULONG ref = InterlockedIncrement(&This->ref);
    TRACE("(%p)->(%d)\n", This, ref);
    return ref;
}

static ULONG WINAPI xmlreaderinput_Release(IXmlReaderInput *iface)
{
    xmlreaderinput *This = impl_from_IXmlReaderInput(iface);
    LONG ref = InterlockedDecrement(&This->ref);

    TRACE("(%p)->(%d)\n", This, ref);

    if (ref == 0)
    {
        IMalloc *imalloc = This->imalloc;
        if (This->input) IUnknown_Release(This->input);
        if (This->stream) ISequentialStream_Release(This->stream);
        if (This->buffer) free_input_buffer(This->buffer);
        readerinput_free(This, This);
        if (imalloc) IMalloc_Release(imalloc);
    }

    return ref;
}

static const struct IUnknownVtbl xmlreaderinput_vtbl =
{
    xmlreaderinput_QueryInterface,
    xmlreaderinput_AddRef,
    xmlreaderinput_Release
};

HRESULT WINAPI CreateXmlReader(REFIID riid, void **obj, IMalloc *imalloc)
{
    xmlreader *reader;

    TRACE("(%s, %p, %p)\n", wine_dbgstr_guid(riid), obj, imalloc);

    if (!IsEqualGUID(riid, &IID_IXmlReader))
    {
        ERR("Unexpected IID requested -> (%s)\n", wine_dbgstr_guid(riid));
        return E_FAIL;
    }

    if (imalloc)
        reader = IMalloc_Alloc(imalloc, sizeof(*reader));
    else
        reader = heap_alloc(sizeof(*reader));
    if(!reader) return E_OUTOFMEMORY;

    reader->IXmlReader_iface.lpVtbl = &xmlreader_vtbl;
    reader->ref = 1;
    reader->input = NULL;
    reader->state = XmlReadState_Closed;
    reader->dtdmode = DtdProcessing_Prohibit;
    reader->line  = reader->pos = 0;
    reader->imalloc = imalloc;
    if (imalloc) IMalloc_AddRef(imalloc);

    *obj = &reader->IXmlReader_iface;

    TRACE("returning iface %p\n", *obj);

    return S_OK;
}

HRESULT WINAPI CreateXmlReaderInputWithEncodingName(IUnknown *stream,
                                                    IMalloc *imalloc,
                                                    LPCWSTR encoding,
                                                    BOOL hint,
                                                    LPCWSTR base_uri,
                                                    IXmlReaderInput **ppInput)
{
    xmlreaderinput *readerinput;
    HRESULT hr;

    FIXME("%p %p %s %d %s %p: stub\n", stream, imalloc, wine_dbgstr_w(encoding),
                                       hint, wine_dbgstr_w(base_uri), ppInput);

    if (!stream || !ppInput) return E_INVALIDARG;

    if (imalloc)
        readerinput = IMalloc_Alloc(imalloc, sizeof(*readerinput));
    else
        readerinput = heap_alloc(sizeof(*readerinput));
    if(!readerinput) return E_OUTOFMEMORY;

    readerinput->IXmlReaderInput_iface.lpVtbl = &xmlreaderinput_vtbl;
    readerinput->ref = 1;
    readerinput->imalloc = imalloc;
    readerinput->stream = NULL;
    if (imalloc) IMalloc_AddRef(imalloc);

    hr = alloc_input_buffer(readerinput, XmlEncoding_UTF16);
    if (hr != S_OK)
    {
        readerinput_free(readerinput, readerinput);
        return hr;
    }
    IUnknown_QueryInterface(stream, &IID_IUnknown, (void**)&readerinput->input);

    *ppInput = &readerinput->IXmlReaderInput_iface;

    TRACE("returning iface %p\n", *ppInput);

    return S_OK;
}
