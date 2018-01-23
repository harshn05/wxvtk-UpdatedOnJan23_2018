// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wxVTKRenderWindowInteractor.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkConeSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include <vtkSmartPointer.h>

// the application icon
#ifndef __WXMSW__
    #include "mondrian.xpm"
#endif

class MyApp;
class MyFrame;

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
	virtual bool OnInit();
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~MyFrame();

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

protected:
    void ConstructVTK();
    void ConfigureVTK();
    void DestroyVTK();

private:
  wxVTKRenderWindowInteractor *m_pVTKWindow;

  // vtk classes
  vtkRenderer       *pRenderer;
  vtkRenderWindow   *pRenderWindow;
  vtkPolyDataMapper *pConeMapper;
  vtkActor          *pConeActor;
  vtkConeSource     *pConeSource;

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Quit = 1,
    Minimal_About
};

#define MY_FRAME      101
#define MY_VTK_WINDOW 102

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // create the main application window
    MyFrame *frame = new MyFrame(_T("wxWidgets-VTK App"),
                                 wxPoint(50, 50), wxSize(450, 340));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(TRUE);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
    return TRUE;
}

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
#ifdef __WXMAC__
    // we need this in order to allow the about menu relocation, since ABOUT is
    // not the default id of the about menu
    wxApp::s_macAboutMenuItemId = Minimal_About;
#endif

    // set the frame icon
    SetIcon(wxICON(mondrian));

    // create a menu bar
    wxMenu *menuFile = new wxMenu(_T(""), wxMENU_TEAROFF);

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, _T("&About...\tCtrl-A"), _T("Show about dialog"));

    menuFile->Append(Minimal_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Drag the mouse here! (wxWindows 3.0.3)"));
#endif // wxUSE_STATUSBAR

    m_pVTKWindow = new wxVTKRenderWindowInteractor(this, MY_VTK_WINDOW);
    m_pVTKWindow->UseCaptureMouseOn();
//    m_pVTKWindow->DebugOn();
	
    ConstructVTK();
    ConfigureVTK();
}

MyFrame::~MyFrame()
{
  if(m_pVTKWindow) m_pVTKWindow->Delete();
  DestroyVTK();
}

void MyFrame::ConstructVTK()
{
  pRenderer     = vtkRenderer::New();
  pConeMapper   = vtkPolyDataMapper::New();
  pConeActor    = vtkActor::New();
  pConeSource   = vtkConeSource::New();
  
}

void MyFrame::ConfigureVTK()
{
  // connect the render window and wxVTK window
  pRenderWindow = m_pVTKWindow->GetRenderWindow();

  // connect renderer and render window and configure render window
  pRenderWindow->AddRenderer(pRenderer);

  // initialize cone
  pConeSource->SetResolution(100);

  // connect pipeline
  pConeMapper->SetInputConnection(pConeSource->GetOutputPort());
  pConeActor->SetMapper(pConeMapper);
  pRenderer->AddActor(pConeActor);

  // configure renderer
  pRenderer->SetBackground(0.5, 0.5, 0.5);
  pRenderer->GradientBackgroundOn();
  pRenderer->SetBackground(1, 1, 1);
  pRenderer->SetBackground2(0, 0, 1);
  pRenderer->GetActiveCamera()->Elevation(30.0);
  pRenderer->GetActiveCamera()->Azimuth(30.0);
  pRenderer->GetActiveCamera()->Zoom(1.0);
  //pRenderer->GetActiveCamera()->SetClippingRange(1,1000);
 }

void MyFrame::DestroyVTK()
{
//http://www.vtk.org/pipermail/vtkusers/2003-September/019894.html
  if (pRenderer != 0)
    pRenderer->Delete();
  if (pConeMapper != 0)
    pConeMapper->Delete();
  if (pConeActor != 0)
    pConeActor->Delete();
  if (pConeSource != 0)
    pConeSource->Delete();
}

// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("This is the about dialog of wx-vtk sample.\n"));

    wxMessageBox(msg, _T("About wx-vtk"), wxOK | wxICON_INFORMATION, this);
}
