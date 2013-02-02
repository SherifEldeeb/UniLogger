![My image](http://eldeeb.net/wrdprs/wp-content/uploads/2013/02/03.png)

http://eldeeb.net/wrdprs/?page_id=229


UniLogger
=========
A "not-evil" keylogger with Unicode support


What?

UniLogger is a keylogger with unicode support “unlike most of keyloggers out there” it supports logging Hebrew, Arabic, Russian, Chinese et’all … technical details on how I did that is below…
It doesn’t require admin rights to operate “like hook-based keyloggers do”.
Along with keystrokes, It logs time, date, and the name of the active application.


Is it evil?

No, it is not; UniLogger runs in the foreground just like any regular program, it  stops logging when you click on it or click the “close” button, … so, and I am talking to you little kid, this program is not going to help you much spying on your friends, go to hackforums for programs to help you do that.


Why?

That other day I discovered that most of the keyloggers do not support Unicode letters, and I couldn’t find a straight forward way to do it in C/C++ … so, when I figured it out, I decided to share my findings with you guys  .


How?

There are two common “not the only” ways to log keys:

Hooking Keyboard “SetWindowsHookEx() & WH_KEYBOARD_LL & WM_CHAR”
This is the most efficient and reliable way to do it, and when the hook will used to process WM_CHAR messages, Windows will do all the heavy lifting and give you the keystroke in its Unicode form directly.
However, it might require elevated privileges, also this method is guaranteed to give you AV’s attention in case you’re trying to avoid that.
GetAsyncKeyState()
The classic way to do it, it works with restricted accounts, yet I have never came across an application that use this method and do the necessary work to log the keystroke in its current language “e.g. if the Arabic character “ش” is pressed, it will log “A” not “ش”
In UniLogger, I used the GetAsyncKeyState() method … the trick I did is I get the language of the window first, then translate that keystroke into the corresponding Unicode letter using the function ToUnicode().


How it is done in UniLogger

Please, give me your attention, this is going to be a rough ride … I learned all the below the hard way!!:

When a key is pressed, GetAsyncKeyState takes what is called “scanned key”
We need something called “Virtual Key” from the “scanned key” … for that we we use “MapVirtualKeyEx”.
However, “MapVirtualKeyEx” needs something called “keyboard layout” to function … so, we get the keyboard layout using “GetKeyboardLayout”
However!! GetKeyboardLayout requires the “thread” of the window that the key was pressed on, for that we use “GetWindowThreadProcessId”
…  however, GetWindowThreadProcessId needs the handle of the process, for that we use GetForegroundWindow().
Finally, we need something called “Keyboard State” … and that’s by using GetKeyboardState() … this one has no “howevers”.
Now we have the following: “ScannedKey, VirtualKey, KeyBoardLayOut, KeyBoardState”
ToUnicodeEx will convert all the above to a unicode character 


