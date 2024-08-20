/* WinRT Windows.Devices.Enumeration.Pnp implementation
 *
 * Copyright 2024 Onni Kukkonen
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

#include "initguid.h"
#include "private.h"
#include <assert.h>

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(enumerationpnp);

struct pnpstatic
{
    IActivationFactory IActivationFactory_iface;
    IPnpObjectStatics IPnpObjectStatics_iface;
    LONG ref;
};

static inline struct pnpstatic *impl_from_IPnpObjectStatics( IPnpObjectStatics *iface )
{
    return CONTAINING_RECORD( iface, struct pnpstatic, IPnpObjectStatics_iface );
}

static HRESULT WINAPI pnpstatic_QueryInterface( IPnpObjectStatics *iface, REFIID iid, void **out )
{
    struct pnpstatic *impl = impl_from_IPnpObjectStatics( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IPnpObjectStatics ))
    {
        *out = &impl->IPnpObjectStatics_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IPnpObjectStatics ))
    {
        *out = &impl->IPnpObjectStatics_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI pnpstatic_AddRef( IPnpObjectStatics *iface )
{
    struct pnpstatic *impl = impl_from_IPnpObjectStatics( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI pnpstatic_Release( IPnpObjectStatics *iface )
{
    struct pnpstatic *impl = impl_from_IPnpObjectStatics( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI pnpstatic_GetIids( IPnpObjectStatics *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI pnpstatic_GetRuntimeClassName( IPnpObjectStatics *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI pnpstatic_GetTrustLevel( IPnpObjectStatics *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI pnpstatic_CreateFromIdAsync( IPnpObjectStatics *iface, PnpObjectType type, HSTRING id, IIterable_HSTRING* requestedProperties, IAsyncOperation_PnpObject** asyncOp)
{
    FIXME( "iface %p, type %04x, id %s, requestedProperties %p, asyncOp %p stub!\n", iface, type, debugstr_hstring(id), requestedProperties, asyncOp );
    return E_NOTIMPL;
}

static HRESULT WINAPI pnpstatic_FindAllAsync( IPnpObjectStatics *iface, PnpObjectType type, IIterable_HSTRING* requestedProperties, IAsyncOperation_PnpObjectCollection** asyncOp ) 
{
    FIXME( "iface %p, type %04x, requestedProperties %p, asyncOp %p stub!\n", iface, type, requestedProperties, asyncOp );
    return E_NOTIMPL;
}

static HRESULT WINAPI pnpstatic_FindAllAsyncAqsFilter( IPnpObjectStatics *iface, PnpObjectType type, IIterable_HSTRING* requestedProperties, HSTRING aqsFilter, IAsyncOperation_PnpObjectCollection** asyncOp ) 
{
    FIXME( "iface %p, type %04x, requestedProperties %p, aqsFilter %s, asyncOp %p stub!\n", iface, type, requestedProperties, debugstr_hstring(aqsFilter), asyncOp );
    return E_NOTIMPL;
}

static HRESULT WINAPI pnpstatic_CreateWatcher( IPnpObjectStatics *iface, PnpObjectType type, IIterable_HSTRING* requestedProperties, __x_ABI_CWindows_CDevices_CEnumeration_CPnp_CIPnpObjectWatcher** watcher)
{
    FIXME( "iface %p, type %04x, requestedProperties %p, watcher %p stub!\n", iface, type, requestedProperties, watcher);
    return E_NOTIMPL;
}

static HRESULT WINAPI pnpstatic_CreateWatcherAqsFilter( IPnpObjectStatics *iface, PnpObjectType type, IIterable_HSTRING* requestedProperties, HSTRING aqsFilter, __x_ABI_CWindows_CDevices_CEnumeration_CPnp_CIPnpObjectWatcher** watcher)
{
    FIXME( "iface %p, type %04x, requestedProperties %p, aqsFilter %s, watcher %p stub!\n", iface, type, requestedProperties, debugstr_hstring(aqsFilter), watcher);
    return E_NOTIMPL;
}

//DEFINE_IINSPECTABLE(pnpstatic, IPnpObjectStatics, struct pnpstatic, IActivationFactory_iface);

static const struct IPnpObjectStaticsVtbl pnpstatic_vtbl =
{
    pnpstatic_QueryInterface,
    pnpstatic_AddRef,
    pnpstatic_Release,
    /* IInspectable methods */
    pnpstatic_GetIids,
    pnpstatic_GetRuntimeClassName,
    pnpstatic_GetTrustLevel,
    /* IPnpObjectStatics methods */
    pnpstatic_CreateFromIdAsync,
    pnpstatic_FindAllAsync,
    pnpstatic_FindAllAsyncAqsFilter,
    pnpstatic_CreateWatcher,
    pnpstatic_CreateWatcherAqsFilter
};

static inline struct pnpstatic *impl_from_IActivationFactory(IActivationFactory *iface)
{
    return CONTAINING_RECORD(iface, struct pnpstatic, IActivationFactory_iface);
}

static HRESULT WINAPI factory_QueryInterface(
        IActivationFactory *iface, REFIID iid, void **out)
{
    struct pnpstatic *factory = impl_from_IActivationFactory(iface);

    FIXME("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);

    if (IsEqualGUID(iid, &IID_IUnknown) ||
        IsEqualGUID(iid, &IID_IInspectable) ||
        IsEqualGUID(iid, &IID_IActivationFactory))
    {
        IUnknown_AddRef(iface);
        *out = &factory->IActivationFactory_iface;
        return S_OK;
    }

    if (IsEqualGUID(iid, &IID_IPnpObjectStatics))
    {
        IUnknown_AddRef(iface);
        *out = &factory->IPnpObjectStatics_iface;
        return S_OK;
    }

    FIXME("%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI factory_AddRef(IActivationFactory *iface)
{
    struct pnpstatic *factory = impl_from_IActivationFactory(iface);
    ULONG refcount = InterlockedIncrement(&factory->ref);

    TRACE("iface %p, refcount %lu.\n", iface, refcount);

    return refcount;
}

static ULONG WINAPI factory_Release(IActivationFactory *iface)
{
    struct pnpstatic *factory = impl_from_IActivationFactory(iface);
    ULONG refcount = InterlockedDecrement(&factory->ref);

    TRACE("iface %p, refcount %lu.\n", iface, refcount);

    return refcount;
}

static HRESULT WINAPI factory_GetIids(
        IActivationFactory *iface, ULONG *iid_count, IID **iids)
{
    FIXME("iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids);
    return E_NOTIMPL;
}

static HRESULT WINAPI factory_GetRuntimeClassName(
        IActivationFactory *iface, HSTRING *class_name)
{
    FIXME("iface %p, class_name %p stub!\n", iface, class_name);
    return E_NOTIMPL;
}

static HRESULT WINAPI factory_GetTrustLevel(
        IActivationFactory *iface, TrustLevel *trust_level)
{
    FIXME("iface %p, trust_level %p stub!\n", iface, trust_level);
    return E_NOTIMPL;
}

static HRESULT WINAPI factory_ActivateInstance(
        IActivationFactory *iface, IInspectable **instance)
{
    FIXME("iface %p, instance %p stub!\n", iface, instance);
    return E_NOTIMPL;
}

static const struct IActivationFactoryVtbl factory_vtbl =
{
    factory_QueryInterface,
    factory_AddRef,
    factory_Release,
    /* IInspectable methods */
    factory_GetIids,
    factory_GetRuntimeClassName,
    factory_GetTrustLevel,
    /* IActivationFactory methods */
    factory_ActivateInstance,
};

static struct pnpstatic pnpstatic_global =
{
    .IActivationFactory_iface.lpVtbl = &factory_vtbl,
    .IPnpObjectStatics_iface.lpVtbl = &pnpstatic_vtbl,
    .ref = 1,
};

IActivationFactory *activation_factory = &pnpstatic_global.IActivationFactory_iface;

HRESULT WINAPI DllGetClassObject( REFCLSID clsid, REFIID riid, void **out )
{
    FIXME( "clsid %s, riid %s, out %p stub!\n", debugstr_guid( clsid ), debugstr_guid( riid ), out );
    return CLASS_E_CLASSNOTAVAILABLE;
}

HRESULT WINAPI DllGetActivationFactory( HSTRING classid, IActivationFactory **factory )
{
    const WCHAR *buffer = WindowsGetStringRawBuffer( classid, NULL );

    TRACE( "class %s, factory %p.\n", debugstr_hstring(classid), factory );

    *factory = NULL;

    if (!wcscmp(buffer, RuntimeClass_Windows_Devices_Enumeration_Pnp_PnpObject))
        IActivationFactory_QueryInterface( activation_factory, &IID_IActivationFactory, (void **)factory );

    if (*factory) return S_OK;
    return CLASS_E_CLASSNOTAVAILABLE;
}
