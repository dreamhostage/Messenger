  This project contains simple messenger with GUI on QT which engine was written on boost/asio. Also there is GUI server, hooking dynamic library file (written by mhook library) and Windows service.
Here is the principles how does all this work:

- Messenger connects users between themselves;
- Dll attaches into the messenger;
- Then Dll hooks messages and then sends them to windows service;
- Windows service sends messages to the GUI server;
- GUI server is able to show messages which users sends to each other and change them by sending changed messages back to the service from where they come in Dll and then back in messenger.
