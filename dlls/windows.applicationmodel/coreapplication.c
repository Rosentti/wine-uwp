/* WinRT Windows.ApplicationModel.Core.CoreApplication implementation
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

#include "private.h"
#include <assert.h>
#include <winuser.h>

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(model);

struct coreapp_impl
{
    IActivationFactory IActivationFactory_iface;
    ICoreApplication ICoreApplication_iface;
    ICoreApplicationView ICoreApplicationView_iface;
    ICoreWindow ICoreWindow_iface;
    ICoreWindowInterop ICoreWindowInterop_iface;
    IFrameworkView *current_view;
    HWND window_handle;
    HANDLE dispatcher_handle;
    LONG ref;
};

static inline struct coreapp_impl *impl_from_IActivationFactory(IActivationFactory *iface)
{
    return CONTAINING_RECORD(iface, struct coreapp_impl, IActivationFactory_iface);
}

static HRESULT WINAPI factory_QueryInterface(
        IActivationFactory *iface, REFIID iid, void **out)
{
    struct coreapp_impl *factory = impl_from_IActivationFactory(iface);

    FIXME("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);

    if (IsEqualGUID(iid, &IID_IUnknown) ||
        IsEqualGUID(iid, &IID_IInspectable) ||
        IsEqualGUID(iid, &IID_IActivationFactory))
    {
        IUnknown_AddRef(iface);
        *out = &factory->IActivationFactory_iface;
        return S_OK;
    }

    if (IsEqualGUID(iid, &IID_ICoreApplication))
    {
        IUnknown_AddRef(iface);
        *out = &factory->ICoreApplication_iface;
        return S_OK;
    }

    FIXME("%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI factory_AddRef(IActivationFactory *iface)
{
    struct coreapp_impl *factory = impl_from_IActivationFactory(iface);
    ULONG refcount = InterlockedIncrement(&factory->ref);

    TRACE("iface %p, refcount %lu.\n", iface, refcount);

    return refcount;
}

static ULONG WINAPI factory_Release(IActivationFactory *iface)
{
    struct coreapp_impl *factory = impl_from_IActivationFactory(iface);
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


static inline struct coreapp_impl *impl_from_ICoreApplication( ICoreApplication *iface )
{
    return CONTAINING_RECORD( iface, struct coreapp_impl, ICoreApplication_iface );
}

static HRESULT WINAPI coreapp_impl_QueryInterface( ICoreApplication *iface, REFIID iid, void **out )
{
    struct coreapp_impl *impl = impl_from_ICoreApplication( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_ICoreApplication ))
    {
        *out = &impl->ICoreApplication_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI coreapp_impl_AddRef( ICoreApplication *iface )
{
    struct coreapp_impl *impl = impl_from_ICoreApplication( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI coreapp_impl_Release( ICoreApplication *iface )
{
    struct coreapp_impl *impl = impl_from_ICoreApplication( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI coreapp_impl_GetIids( ICoreApplication *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI coreapp_impl_GetRuntimeClassName( ICoreApplication *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI coreapp_impl_GetTrustLevel( ICoreApplication *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI coreapp_impl_get_Id( ICoreApplication *iface, HSTRING *value)
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    return E_NOTIMPL;
}

static HRESULT WINAPI coreapp_impl_add_Suspending( ICoreApplication *iface, IEventHandler_SuspendingEventArgs *handler, EventRegistrationToken *token)
{
    FIXME("iface %p, handler %p, token %p stub.\n", iface, handler, token);
    return S_OK;
}

static HRESULT WINAPI coreapp_impl_remove_Suspending( ICoreApplication *iface, EventRegistrationToken token)
{
    FIXME("iface %p, token %#I64x stub.\n", iface, token.value);
    return E_NOTIMPL;
}

static HRESULT WINAPI coreapp_impl_add_Resuming( ICoreApplication *iface, IEventHandler_IInspectable *handler, EventRegistrationToken *token)
{
    FIXME("iface %p, handler %p, token %p stub.\n", iface, handler, token);
    return S_OK;
}

static HRESULT WINAPI coreapp_impl_remove_Resuming( ICoreApplication *iface, EventRegistrationToken token)
{
    FIXME("iface %p, token %#I64x stub.\n", iface, token.value);
    return E_NOTIMPL;
}

static HRESULT WINAPI coreapp_impl_get_Properties( ICoreApplication *iface, IPropertySet **value)
{
    FIXME("iface %p, value %p stub.\n", iface, value);
    return E_NOTIMPL;
}

static HRESULT WINAPI coreapp_impl_GetCurrentView( ICoreApplication *iface, ICoreApplicationView **value)
{
    FIXME("iface %p, value %p stub.\n", iface, value);
    return E_NOTIMPL;
}

// this is the main message handler for the program
static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // sort through and find what code to run for the message given
    switch(message)
    {
        // this message is read when the window is closed
        case WM_DESTROY:
            {
                // close the application entirely
                PostQuitMessage(0);
                return 0;
            } break;
    }

    // Handle any messages the switch statement didn't
    return DefWindowProcW(hWnd, message, wParam, lParam);
}

static DWORD WINAPI DispatcherMain(void* data) {
  MSG msg;

  struct coreapp_impl *impl = data;

  impl->window_handle = CreateWindowExW(WS_EX_APPWINDOW, L"UWPWindowClass", L"UWP Window", WS_OVERLAPPEDWINDOW, 0, 0, 1920, 1080, NULL, NULL, NULL, NULL);
  ShowWindow(impl->window_handle, SW_SHOW);

  while(GetMessageW(&msg, NULL, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }

  return 0;
}

static HRESULT WINAPI coreapp_impl_Run( ICoreApplication *iface, IFrameworkViewSource *view_source)
{
    HRESULT ret;
    HSTRING handle;
    WNDCLASSEXW wc;

    struct coreapp_impl *impl = impl_from_ICoreApplication( iface );

    FIXME("iface %p, view_source %p stub.\n", iface, view_source);
    
    ret = view_source->lpVtbl->CreateView(view_source, &impl->current_view);
    if (ret != S_OK) {
        ERR("CreateView fail\n");
        return ret;
    }

    // Initialize, Load, Run
    impl->current_view->lpVtbl->Initialize(impl->current_view, &impl->ICoreApplicationView_iface);

    ret = WindowsCreateString(NULL, 0, &handle);
    if (ret != S_OK) {
        ERR("WindowsCreateString fail\n");
        return ret;
    }

    ZeroMemory(&wc, sizeof(WNDCLASSEXW));

    // fill in the struct with the needed information
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = NULL;
    wc.hCursor = LoadCursorW(NULL, (LPCWSTR)IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = L"UWPWindowClass";

    // register the window class
    RegisterClassExW(&wc);

    impl->dispatcher_handle = CreateThread(NULL, 0, DispatcherMain, impl, 0, NULL);
    
    impl->current_view->lpVtbl->SetWindow(impl->current_view, &impl->ICoreWindow_iface);
    impl->current_view->lpVtbl->Load(impl->current_view, handle);

    //TODO: Run Activated handler
    impl->current_view->lpVtbl->Run(impl->current_view);
    return S_OK;
}

static HRESULT WINAPI coreapp_impl_RunWithActivationFactories( ICoreApplication *iface, IGetActivationFactory *factory)
{
    FIXME("iface %p, factory %p stub.\n", iface, factory);
    return E_NOTIMPL;
}

static const struct ICoreApplicationVtbl coreapp_impl_vtbl =
{
    coreapp_impl_QueryInterface,
    coreapp_impl_AddRef,
    coreapp_impl_Release,
    /* IInspectable methods */
    coreapp_impl_GetIids,
    coreapp_impl_GetRuntimeClassName,
    coreapp_impl_GetTrustLevel,
    /* ICoreApplication methods */
    coreapp_impl_get_Id,
    coreapp_impl_add_Suspending,
    coreapp_impl_remove_Suspending,
    coreapp_impl_add_Resuming,
    coreapp_impl_remove_Resuming,
    coreapp_impl_get_Properties,
    coreapp_impl_GetCurrentView,
    coreapp_impl_Run,
    coreapp_impl_RunWithActivationFactories
};

static inline struct coreapp_impl *impl_from_ICoreWindowInterop( ICoreWindowInterop *iface )
{
    return CONTAINING_RECORD( iface, struct coreapp_impl, ICoreWindowInterop_iface );
}

static HRESULT WINAPI corewindow_interop_impl_QueryInterface( ICoreWindowInterop *iface, REFIID iid, void **out )
{
    struct coreapp_impl *impl = impl_from_ICoreWindowInterop( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_ICoreWindowInterop ))
    {
        *out = &impl->ICoreWindowInterop_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI corewindow_interop_impl_AddRef( ICoreWindowInterop *iface )
{
    struct coreapp_impl *impl = impl_from_ICoreWindowInterop( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI corewindow_interop_impl_Release( ICoreWindowInterop *iface )
{
    struct coreapp_impl *impl = impl_from_ICoreWindowInterop( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI corewindow_interop_impl_get_WindowHandle( ICoreWindowInterop *iface, HWND *value)
{
    struct coreapp_impl *impl = impl_from_ICoreWindowInterop( iface );
    *value = impl->window_handle;
    return S_OK;
}

static HRESULT WINAPI corewindow_interop_impl_set_MessageHandled( ICoreWindowInterop *iface, boolean value)
{
    FIXME("iface %p, value %d stub.\n", iface, value);
    return S_OK;
}

static const struct ICoreWindowInteropVtbl corewindow_interop_impl_vtbl =
{
    corewindow_interop_impl_QueryInterface,
    corewindow_interop_impl_AddRef,
    corewindow_interop_impl_Release,
    /* ICoreWindowInterop methods */
    corewindow_interop_impl_get_WindowHandle,
    corewindow_interop_impl_set_MessageHandled,
};

static inline struct coreapp_impl *impl_from_ICoreApplicationView( ICoreApplicationView *iface )
{
    return CONTAINING_RECORD( iface, struct coreapp_impl, ICoreApplicationView_iface );
}

static HRESULT WINAPI coreappview_impl_QueryInterface( ICoreApplicationView *iface, REFIID iid, void **out )
{
    struct coreapp_impl *impl = impl_from_ICoreApplicationView( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_ICoreApplicationView ))
    {
        *out = &impl->ICoreApplicationView_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI coreappview_impl_AddRef( ICoreApplicationView *iface )
{
    struct coreapp_impl *impl = impl_from_ICoreApplicationView( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI coreappview_impl_Release( ICoreApplicationView *iface )
{
    struct coreapp_impl *impl = impl_from_ICoreApplicationView( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI coreappview_impl_GetIids( ICoreApplicationView *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI coreappview_impl_GetRuntimeClassName( ICoreApplicationView *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI coreappview_impl_GetTrustLevel( ICoreApplicationView *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI coreappview_impl_get_CoreWindow( ICoreApplicationView *iface, __x_ABI_CWindows_CUI_CCore_CICoreWindow **value)
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    return E_NOTIMPL;
}

static HRESULT WINAPI coreappview_impl_add_Activated( ICoreApplicationView *iface, ITypedEventHandler_CoreApplicationView_IActivatedEventArgs *handler, EventRegistrationToken *token)
{
    FIXME("iface %p, handler %p, token %p stub.\n", iface, handler, token);
    return S_OK;
}

static HRESULT WINAPI coreappview_impl_remove_Activated( ICoreApplicationView *iface, EventRegistrationToken token)
{
    FIXME("iface %p, token %#I64x stub.\n", iface, token.value);
    return E_NOTIMPL;
}

static HRESULT WINAPI coreappview_impl_get_IsMain( ICoreApplicationView *iface, boolean *value)
{
    FIXME("iface %p, value %p stub.\n", iface, value);
    return E_NOTIMPL;
}

static HRESULT WINAPI coreappview_impl_get_IsHosted( ICoreApplicationView *iface, boolean *value)
{
    FIXME("iface %p, value %p stub.\n", iface, value);
    return E_NOTIMPL;
}

static const struct ICoreApplicationViewVtbl coreappview_impl_vtbl =
{
    coreappview_impl_QueryInterface,
    coreappview_impl_AddRef,
    coreappview_impl_Release,
    /* IInspectable methods */
    coreappview_impl_GetIids,
    coreappview_impl_GetRuntimeClassName,
    coreappview_impl_GetTrustLevel,
    /* ICoreApplicationView methods */
    coreappview_impl_get_CoreWindow,
    coreappview_impl_add_Activated,
    coreappview_impl_remove_Activated,
    coreappview_impl_get_IsMain,
    coreappview_impl_get_IsHosted
};

static inline struct coreapp_impl *impl_from_ICoreWindow( ICoreWindow *iface )
{
    return CONTAINING_RECORD( iface, struct coreapp_impl, ICoreWindow_iface );
}

static HRESULT WINAPI corewindow_impl_QueryInterface( ICoreWindow *iface, REFIID iid, void **out )
{
    struct coreapp_impl *impl = impl_from_ICoreWindow( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_ICoreWindow ))
    {
        *out = &impl->ICoreWindow_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_ICoreWindowInterop ))
    {
        *out = &impl->ICoreWindowInterop_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI corewindow_impl_AddRef( ICoreWindow *iface )
{
    struct coreapp_impl *impl = impl_from_ICoreWindow( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI corewindow_impl_Release( ICoreWindow *iface )
{
    struct coreapp_impl *impl = impl_from_ICoreWindow( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI corewindow_impl_GetIids( ICoreWindow *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI corewindow_impl_GetRuntimeClassName( ICoreWindow *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI corewindow_impl_GetTrustLevel( ICoreWindow *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI corewindow_impl_get_AutomationHostProvider( ICoreWindow *iface, IInspectable **value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    return E_NOTIMPL;
}

static HRESULT WINAPI corewindow_impl_get_Bounds( ICoreWindow *iface, Rect *value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    value->X = 0;
    value->Y = 0;
    value->Width = 1980;
    value->Height = 1080;
    return S_OK;
}

static HRESULT WINAPI corewindow_impl_get_CustomProperties( ICoreWindow *iface, IPropertySet **value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    return E_NOTIMPL;
}

static HRESULT WINAPI corewindow_impl_get_Dispatcher( ICoreWindow *iface, ICoreDispatcher **value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    return E_NOTIMPL;
}

static HRESULT WINAPI corewindow_impl_get_FlowDirection( ICoreWindow *iface, CoreWindowFlowDirection *value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    *value = CoreWindowFlowDirection_LeftToRight;
    return S_OK;
}

static HRESULT WINAPI corewindow_impl_set_FlowDirection( ICoreWindow *iface, CoreWindowFlowDirection value )
{
    FIXME( "iface %p, value %d stub!\n", iface, value );
    return S_OK;
}

static HRESULT WINAPI corewindow_impl_get_IsInputEnabled( ICoreWindow *iface, boolean *value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    *value = TRUE;
    return S_OK;
}

static HRESULT WINAPI corewindow_impl_set_IsInputEnabled( ICoreWindow *iface, boolean value )
{
    FIXME( "iface %p, value %d stub!\n", iface, value );
    return S_OK;
}

static HRESULT WINAPI corewindow_impl_get_PointerCursor( ICoreWindow *iface, ICoreCursor **value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    return E_NOTIMPL;
}

static HRESULT WINAPI corewindow_impl_set_PointerCursor( ICoreWindow *iface, ICoreCursor* value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    return S_OK;
}

static HRESULT WINAPI corewindow_impl_get_PointerPosition( ICoreWindow *iface, Point* value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    return E_NOTIMPL;
}

static HRESULT WINAPI corewindow_impl_get_Visible( ICoreWindow *iface, boolean* value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    *value = TRUE;
    return S_OK;
}

static HRESULT WINAPI corewindow_impl_Activate( ICoreWindow *iface )
{
    FIXME( "iface %p stub!\n", iface );
    return S_OK;
}

static HRESULT WINAPI corewindow_impl_Close( ICoreWindow *iface )
{
    FIXME( "iface %p stub!\n", iface );
    return S_OK;
}

static HRESULT WINAPI corewindow_impl_GetAsyncKeyState( ICoreWindow *iface, VirtualKey key, CoreVirtualKeyStates *state)
{
    FIXME( "iface %p stub!\n", iface );
    return E_NOTIMPL;
}

static HRESULT WINAPI corewindow_impl_GetKeyState( ICoreWindow *iface, VirtualKey key, CoreVirtualKeyStates *state)
{
    FIXME( "iface %p stub!\n", iface );
    return E_NOTIMPL;
}

static HRESULT WINAPI corewindow_impl_ReleasePointerCapture( ICoreWindow *iface )
{
    FIXME( "iface %p stub!\n", iface );
    return S_OK;
}

static HRESULT WINAPI corewindow_impl_SetPointerCapture( ICoreWindow *iface )
{
    FIXME( "iface %p stub!\n", iface );
    return S_OK;
}

DEFINE_EVENT_STUB(CoreWindow, corewindow_impl, Activated, WindowActivatedEventArgs)
DEFINE_EVENT_STUB(CoreWindow, corewindow_impl, AutomationProviderRequested, AutomationProviderRequestedEventArgs)
DEFINE_EVENT_STUB(CoreWindow, corewindow_impl, CharacterReceived, CharacterReceivedEventArgs)
DEFINE_EVENT_STUB(CoreWindow, corewindow_impl, Closed, CoreWindowEventArgs)
DEFINE_EVENT_STUB(CoreWindow, corewindow_impl, InputEnabled, InputEnabledEventArgs)
DEFINE_EVENT_STUB(CoreWindow, corewindow_impl, KeyDown, KeyEventArgs)
DEFINE_EVENT_STUB(CoreWindow, corewindow_impl, KeyUp, KeyEventArgs)
DEFINE_EVENT_STUB(CoreWindow, corewindow_impl, PointerCaptureLost, PointerEventArgs)
DEFINE_EVENT_STUB(CoreWindow, corewindow_impl, PointerEntered, PointerEventArgs)
DEFINE_EVENT_STUB(CoreWindow, corewindow_impl, PointerExited, PointerEventArgs)
DEFINE_EVENT_STUB(CoreWindow, corewindow_impl, PointerMoved, PointerEventArgs)
DEFINE_EVENT_STUB(CoreWindow, corewindow_impl, PointerPressed, PointerEventArgs)
DEFINE_EVENT_STUB(CoreWindow, corewindow_impl, PointerReleased, PointerEventArgs)
DEFINE_EVENT_STUB(CoreWindow, corewindow_impl, TouchHitTesting, TouchHitTestingEventArgs)
DEFINE_EVENT_STUB(CoreWindow, corewindow_impl, PointerWheelChanged, PointerEventArgs)
DEFINE_EVENT_STUB(CoreWindow, corewindow_impl, SizeChanged, WindowSizeChangedEventArgs)
DEFINE_EVENT_STUB(CoreWindow, corewindow_impl, VisibilityChanged, VisibilityChangedEventArgs)

static const struct ICoreWindowVtbl corewindow_impl_vtbl =
{
    corewindow_impl_QueryInterface,
    corewindow_impl_AddRef,
    corewindow_impl_Release,
    /* IInspectable methods */
    corewindow_impl_GetIids,
    corewindow_impl_GetRuntimeClassName,
    corewindow_impl_GetTrustLevel,
    /* ICoreWindow methods */
    corewindow_impl_get_AutomationHostProvider,
    corewindow_impl_get_Bounds,
    corewindow_impl_get_CustomProperties,
    corewindow_impl_get_Dispatcher,
    corewindow_impl_get_FlowDirection,
    corewindow_impl_set_FlowDirection,
    corewindow_impl_get_IsInputEnabled,
    corewindow_impl_set_IsInputEnabled,
    corewindow_impl_get_PointerCursor,
    corewindow_impl_set_PointerCursor,
    corewindow_impl_get_PointerPosition,
    corewindow_impl_get_Visible,
    corewindow_impl_Activate,
    corewindow_impl_Close,
    corewindow_impl_GetAsyncKeyState,
    corewindow_impl_GetKeyState,
    corewindow_impl_ReleasePointerCapture,
    corewindow_impl_SetPointerCapture,
    DECLARE_EVENT(corewindow_impl, Activated),
    DECLARE_EVENT(corewindow_impl, AutomationProviderRequested),
    DECLARE_EVENT(corewindow_impl, CharacterReceived),
    DECLARE_EVENT(corewindow_impl, Closed),
    DECLARE_EVENT(corewindow_impl, InputEnabled),
    DECLARE_EVENT(corewindow_impl, KeyDown),
    DECLARE_EVENT(corewindow_impl, KeyUp),
    DECLARE_EVENT(corewindow_impl, PointerCaptureLost),
    DECLARE_EVENT(corewindow_impl, PointerEntered),
    DECLARE_EVENT(corewindow_impl, PointerExited),
    DECLARE_EVENT(corewindow_impl, PointerMoved),
    DECLARE_EVENT(corewindow_impl, PointerPressed),
    DECLARE_EVENT(corewindow_impl, PointerReleased),
    DECLARE_EVENT(corewindow_impl, TouchHitTesting),
    DECLARE_EVENT(corewindow_impl, PointerWheelChanged),
    DECLARE_EVENT(corewindow_impl, SizeChanged),
    DECLARE_EVENT(corewindow_impl, VisibilityChanged)
};

static struct coreapp_impl coreapp_impl_global =
{
    .IActivationFactory_iface.lpVtbl = &factory_vtbl,
    .ICoreApplication_iface.lpVtbl = &coreapp_impl_vtbl,
    .ICoreApplicationView_iface.lpVtbl = &coreappview_impl_vtbl,
    .ICoreWindow_iface.lpVtbl = &corewindow_impl_vtbl,
    .ICoreWindowInterop_iface.lpVtbl = &corewindow_interop_impl_vtbl,
    .ref = 1,
};

IActivationFactory *coreapplication_factory = &coreapp_impl_global.IActivationFactory_iface;
