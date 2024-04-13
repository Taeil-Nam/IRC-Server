/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "GameRC (IRC server)", "index.html", [
    [ "💬 GameRC 프로젝트", "index.html", "index" ],
    [ "📄 IRC 프로토콜", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html", [
      [ "Table of Contents", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md21", [
        [ "- @ref \"62-command-responses\" \"6.2. Command responses\"", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md22", null ],
        [ "1. INTRODUCTION", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md23", [
          [ "1.1. Servers", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md25", null ],
          [ "1.2. Clients", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md27", [
            [ "1.2.1. Operators", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md28", null ]
          ] ],
          [ "1.3. Channels", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md30", [
            [ "1.3.1. Channel Operators", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md31", null ]
          ] ]
        ] ],
        [ "2. THE IRC SPECIFICATION", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md33", [
          [ "2.1. 개요", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md34", null ],
          [ "2.2. 문자 코드 (Character codes)", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md36", null ],
          [ "2.3. Messages", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md38", [
            [ "2.3.1. Message format in 'pseudo' BNF", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md39", null ]
          ] ],
          [ "2.4. Numeric replies", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md41", null ]
        ] ],
        [ "3. IRC Concepts", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md43", [
          [ "3.1. One-to-one communication", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md45", null ],
          [ "3.2. One-to-Many", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md47", [
            [ "3.2.2. To a group (channel)", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md48", null ]
          ] ]
        ] ],
        [ "4. Message details", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md50", [
          [ "4.1. Connection Registration", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md52", [
            [ "4.1.1. Password message", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md53", null ],
            [ "4.1.2. Nick message", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md54", null ],
            [ "4.1.3. User message", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md55", null ],
            [ "4.1.6. Quit", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md56", null ]
          ] ],
          [ "4.2. Channel operations", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md58", [
            [ "4.2.1. Join message", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md59", null ],
            [ "4.2.2. Part message", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md60", null ],
            [ "4.2.3. Mode message", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md61", null ],
            [ "4.2.4. Topic message", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md63", null ],
            [ "4.2.7. Invite message", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md64", null ],
            [ "4.2.8. Kick command", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md65", null ]
          ] ],
          [ "4.4. Sending messages", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md67", [
            [ "4.4.1. Private messages", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md68", null ],
            [ "4.4.2. Notice", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md69", null ]
          ] ],
          [ "4.6 Miscellaneous messages", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md71", [
            [ "4.6.2. Ping message", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md72", null ],
            [ "4.6.3. Pong message", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md73", null ]
          ] ]
        ] ],
        [ "6. REPLIES", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md75", [
          [ "6.1 Error Replies", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md77", null ],
          [ "6.2. Command responses", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md79", null ]
        ] ],
        [ "7. Client and server authentication", "md__2_users_2wasavi_2ft__irc_2markdown_2_i_r_c.html#autotoc_md81", null ]
      ] ]
    ] ],
    [ "🛠️ 아키텍쳐", "md__2_users_2wasavi_2ft__irc_2markdown_2_a_r_c_h_i_t_e_c_t_u_r_e.html", [
      [ "프로젝트 요구사항", "md__2_users_2wasavi_2ft__irc_2markdown_2_a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md84", null ],
      [ "코딩 컨벤션", "md__2_users_2wasavi_2ft__irc_2markdown_2_a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md85", null ],
      [ "협업", "md__2_users_2wasavi_2ft__irc_2markdown_2_a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md86", [
        [ "소스코드 관리", "md__2_users_2wasavi_2ft__irc_2markdown_2_a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md87", null ],
        [ "코드 작성", "md__2_users_2wasavi_2ft__irc_2markdown_2_a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md88", null ]
      ] ],
      [ "구조", "md__2_users_2wasavi_2ft__irc_2markdown_2_a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md89", [
        [ "UI", "md__2_users_2wasavi_2ft__irc_2markdown_2_a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md90", null ],
        [ "Network", "md__2_users_2wasavi_2ft__irc_2markdown_2_a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md91", null ],
        [ "IRC Core", "md__2_users_2wasavi_2ft__irc_2markdown_2_a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md92", null ],
        [ "Logging", "md__2_users_2wasavi_2ft__irc_2markdown_2_a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md93", null ]
      ] ]
    ] ],
    [ "네임스페이스", "namespaces.html", [
      [ "네임스페이스 목록", "namespaces.html", "namespaces_dup" ]
    ] ],
    [ "클래스", "annotated.html", [
      [ "클래스 목록", "annotated.html", "annotated_dup" ],
      [ "클래스 색인", "classes.html", null ],
      [ "클래스 계통도", "hierarchy.html", "hierarchy" ],
      [ "클래스 멤버", "functions.html", [
        [ "모두", "functions.html", "functions_dup" ],
        [ "함수", "functions_func.html", "functions_func" ],
        [ "변수", "functions_vars.html", null ],
        [ "타입정의", "functions_type.html", null ],
        [ "열거형 타입", "functions_enum.html", null ],
        [ "열거형 멤버", "functions_eval.html", null ]
      ] ]
    ] ],
    [ "파일들", "files.html", [
      [ "파일 목록", "files.html", "files_dup" ],
      [ "파일 멤버", "globals.html", [
        [ "모두", "globals.html", null ],
        [ "함수", "globals_func.html", null ],
        [ "타입정의", "globals_type.html", null ],
        [ "매크로", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"_assert_8hpp.html",
"classgdf_1_1_display_console.html#a6f3f00c9baddeabc6c9d79e8ed5d565f",
"classgrc_1_1_channel.html#aabcfc2530d45a4dd08999b83d1dd4da8",
"classgrc_1_1_user_manager.html#a680bb3f97e25765f28711e1a16f1600a"
];

var SYNCONMSG = '패널 동기화를 비활성화하기 위해 클릭하십시오';
var SYNCOFFMSG = '패널 동기화를 활성화하기 위해 클릭하십시오';