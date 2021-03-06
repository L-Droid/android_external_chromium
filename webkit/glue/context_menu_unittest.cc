// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Tests for displaying context menus in corner cases (and swallowing context
// menu events when appropriate)

#include <vector>

#include "base/file_path.h"
#include "base/file_util.h"
#include "base/message_loop.h"
#include "googleurl/src/gurl.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebInputEvent.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebView.h"
#include "webkit/glue/webkit_glue.h"
#include "webkit/tools/test_shell/test_shell_test.h"
#include "webkit/tools/test_shell/test_webview_delegate.h"

using WebKit::WebInputEvent;
using WebKit::WebMouseEvent;
using WebKit::WebView;

// Right clicking inside on an iframe should produce a context menu
class ContextMenuCapturing : public TestShellTest {
 protected:
  void SetUp() {
    TestShellTest::SetUp();

    iframes_data_dir_ = data_dir_;
    iframes_data_dir_ = iframes_data_dir_.AppendASCII("test_shell");
    iframes_data_dir_ = iframes_data_dir_.AppendASCII("iframes");
    ASSERT_TRUE(file_util::PathExists(iframes_data_dir_));
  }

  FilePath iframes_data_dir_;
};


TEST_F(ContextMenuCapturing, ContextMenuCapturing) {
  // Make sure we have no stored mouse event state
  TestWebViewDelegate* test_delegate = test_shell_->delegate();
  test_delegate->clear_captured_context_menu_events();
  EXPECT_EQ(0U, test_delegate->captured_context_menu_events().size());

  GURL test_url = GetTestURL(iframes_data_dir_, "testiframe.html");
  test_shell_->LoadURL(test_url);
  test_shell_->WaitTestFinished();

  // Create a right click in the center of the iframe. (I'm hoping this will
  // make this a bit more robust in case of some other formatting or other bug.)
  WebMouseEvent mouse_event;
  mouse_event.type = WebInputEvent::MouseDown;
  mouse_event.button = WebMouseEvent::ButtonRight;
  mouse_event.x = 250;
  mouse_event.y = 250;
  mouse_event.globalX = 250;
  mouse_event.globalY = 250;

  WebView* webview = test_shell_->webView();
  webview->handleInputEvent(mouse_event);

  // Now simulate the corresponding up event which should display the menu
  mouse_event.type = WebInputEvent::MouseUp;
  webview->handleInputEvent(mouse_event);

  EXPECT_EQ(1U, test_delegate->captured_context_menu_events().size());
}
