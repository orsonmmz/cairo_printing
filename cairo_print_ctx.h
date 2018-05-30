#ifndef _CAIRO_PRINT_CTX_H_
#define _CAIRO_PRINT_CTX_H_

#include <cairo.h>
#include <wx/dcprint.h>

class wxGCDC;

class CAIRO_PRINT_CTX
{
public:
    CAIRO_PRINT_CTX( wxPrinterDC& aPrintDC );
    ~CAIRO_PRINT_CTX();

    cairo_t* GetContext()
    {
        return m_ctx;
    }

    cairo_surface_t* GetSurface()
    {
        return m_surface;
    }

private:
    wxGCDC* m_gcdc;
    cairo_t* m_ctx;
    cairo_surface_t* m_surface;
};

#endif /* _CAIRO_PRINT_CTX_H_ */
