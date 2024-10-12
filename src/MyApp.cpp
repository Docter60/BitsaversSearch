#include "MyApp.h"

#include <string>

#define WINDOW_WIDTH  960
#define WINDOW_HEIGHT 725

JSValueRef OnDownloadButtonClick(JSContextRef ctx, JSObjectRef function,
  JSObjectRef thisObject, size_t argumentCount, 
  const JSValueRef arguments[], JSValueRef* exception) {
  JSStringRef sr = JSValueToStringCopy(ctx, arguments[0], exception);
  size_t maxBufferSize = JSStringGetMaximumUTF8CStringSize(sr);
  char* buffer = new char[maxBufferSize];
  size_t bytesWritten = JSStringGetUTF8CString(sr, buffer, maxBufferSize);
  std::string s(buffer, bytesWritten);
  std::string command;
  #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
  command = "start " + s;
  #elif __APPLE__
  command = "open " + s;
  #elif __linux__
  command = "xdg-open " + s;
  #else
  #error "Unknown compiler"
  #endif
  std::system(command.c_str());

  return JSValueMakeNull(ctx);
}

MyApp::MyApp() {
  ///
  /// Create our main App instance.
  ///
  app_ = App::Create();

  ///
  /// Create a resizable window by passing by OR'ing our window flags with
  /// kWindowFlags_Resizable.
  ///
  window_ = Window::Create(app_->main_monitor(), WINDOW_WIDTH, WINDOW_HEIGHT,
    false, kWindowFlags_Titled | kWindowFlags_Resizable);
  ///
  /// Create our HTML overlay-- we don't care about its initial size and
  /// position because it'll be calculated when we call OnResize() below.
  ///
  overlay_ = Overlay::Create(window_, 1, 1, 0, 0);

  ///
  /// Force a call to OnResize to perform size/layout of our overlay.
  ///
  OnResize(window_.get(), window_->width(), window_->height());

  ///
  /// Load a page into our overlay's View
  ///
  overlay_->view()->LoadURL("file:///app.html");

  ///
  /// Register our MyApp instance as an AppListener so we can handle the
  /// App's OnUpdate event below.
  ///
  app_->set_listener(this);

  ///
  /// Register our MyApp instance as a WindowListener so we can handle the
  /// Window's OnResize event below.
  ///
  window_->set_listener(this);

  ///
  /// Register our MyApp instance as a LoadListener so we can handle the
  /// View's OnFinishLoading and OnDOMReady events below.
  ///
  overlay_->view()->set_load_listener(this);

  ///
  /// Register our MyApp instance as a ViewListener so we can handle the
  /// View's OnChangeCursor and OnChangeTitle events below.
  ///
  overlay_->view()->set_view_listener(this);
}

MyApp::~MyApp() {
}

void MyApp::Run() {
  app_->Run();
}

void MyApp::OnUpdate() {
  ///
  /// This is called repeatedly from the application's update loop.
  ///
  /// You should update any app logic here.
  ///
}

void MyApp::OnClose(ultralight::Window* window) {
  app_->Quit();
}

void MyApp::OnResize(ultralight::Window* window, uint32_t width, uint32_t height) {
  ///
  /// This is called whenever the window changes size (values in pixels).
  ///
  /// We resize our overlay here to take up the entire window.
  ///
  overlay_->Resize(width, height);
}

void MyApp::OnFinishLoading(ultralight::View* caller,
                            uint64_t frame_id,
                            bool is_main_frame,
                            const String& url) {
  ///
  /// This is called when a frame finishes loading on the page.
  ///
}

void MyApp::OnDOMReady(ultralight::View* caller,
                       uint64_t frame_id,
                       bool is_main_frame,
                       const String& url) {
  ///
  /// This is called when a frame's DOM has finished loading on the page.
  ///
  /// This is the best time to setup any JavaScript bindings.
  ///
  // Acquire the JS execution context for the current page.
  auto scoped_context = caller->LockJSContext();
  
  // Typecast to the underlying JSContextRef.
  JSContextRef ctx = (*scoped_context);
  
  // Create a JavaScript String containing the name of our callback.
  JSStringRef name = JSStringCreateWithUTF8CString("dl");

  // Create a garbage-collected JavaScript function that is bound to our
  // native C callback 'OnDownloadButtonClick()'.
  JSObjectRef func = JSObjectMakeFunctionWithCallback(ctx, name, 
                                                      OnDownloadButtonClick);
  
  // Get the global JavaScript object (aka 'window')
  JSObjectRef globalObj = JSContextGetGlobalObject(ctx);

  // Store our function in the page's global JavaScript object so that it
  // accessible from the page as 'OnDownloadButtonClick()'.
  JSObjectSetProperty(ctx, globalObj, name, func, 0, 0);

  // Release the JavaScript String we created earlier.
  JSStringRelease(name);
}

void MyApp::OnChangeCursor(ultralight::View* caller,
                           Cursor cursor) {
  ///
  /// This is called whenever the page requests to change the cursor.
  ///
  /// We update the main window's cursor here.
  ///
  window_->SetCursor(cursor);
}

void MyApp::OnChangeTitle(ultralight::View* caller,
                          const String& title) {
  ///
  /// This is called whenever the page requests to change the title.
  ///
  /// We update the main window's title here.
  ///
  window_->SetTitle(title.utf8().data());
}
