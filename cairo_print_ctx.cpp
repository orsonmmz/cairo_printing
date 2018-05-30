#include "cairo_print_ctx.h"
#include <stdexcept>
#include <wx/dcgraph.h>

#ifdef __WXMSW__
#include <windows.h>
#include <gdiplus.h>
#include <cairo-win32.h>
#endif /* __WXMSW__ */

#ifdef __WXMAC__
#include <ApplicationServices/ApplicationServices.h>

#ifdef __OBJC__
    #import <Cocoa/Cocoa.h>
#endif
#endif /* __WXMAC__ */


CAIRO_PRINT_CTX::CAIRO_PRINT_CTX( wxPrinterDC& aPrinterDC )
    : m_ctx( nullptr ), m_surface( nullptr )
{
    m_gcdc = new wxGCDC( aPrinterDC );
    wxGraphicsContext* gctx = m_gcdc->GetGraphicsContext();

    if( !gctx )
        throw std::runtime_error( "Could not get the Graphics Context" );

#ifdef __WXGTK__
    m_ctx = static_cast<cairo_t*>( gctx->GetNativeContext() );
    m_surface = cairo_get_target( m_ctx );
#endif /* __WXGTK__ */

#ifdef __WXMSW__
    Gdiplus::Graphics* g = static_cast<Gdiplus::Graphics*>( gctx->GetNativeContext() );
    m_hdc = g->GetHDC();
    m_surface = cairo_win32_printing_surface_create( static_cast<HDC>( m_hdc ) );
    m_ctx = cairo_create( m_surface );
#endif /* __WXMSW__ */

#ifdef __WXMAC__
    CGContextRef cg = (CGContextRef) gctx->GetNativeContext();
    m_surface = cairo_quartz_surface_create_for_cg_context( cg, 1024, 1024 );   // TODO size
    m_ctx = cairo_create( m_surface );
#endif /* __WXMAC__ */

    if( cairo_status( m_ctx ) != CAIRO_STATUS_SUCCESS )
        throw std::runtime_error( "Could not create Cairo context" );

    if( cairo_surface_status( m_surface ) != CAIRO_STATUS_SUCCESS )
        throw std::runtime_error( "Could not create Cairo surface" );

    // TODO
    cairo_reference( m_ctx );
    cairo_surface_reference( m_surface );
}


CAIRO_PRINT_CTX::~CAIRO_PRINT_CTX()
{
    cairo_surface_destroy( m_surface );
    cairo_destroy( m_ctx );
    delete m_gcdc;
}


void CAIRO_PRINT_CTX::Release()
{
#ifdef __WXMSW__
    wxCHECK( m_hdc, /*void*/ );
    cairo_surface_show_page( m_surface );
    wxGraphicsContext* gctx = m_gcdc->GetGraphicsContext();
    Gdiplus::Graphics* g = static_cast<Gdiplus::Graphics*>( gctx->GetNativeContext() );
    g->ReleaseHDC( static_cast<HDC>( m_hdc ) );
    m_hdc = nullptr;    // mark as released
#endif /* __WXMSW__ */
}
