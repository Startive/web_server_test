# web server thing

simple little web server i'm making in C++ with linux sys calls :)

## TODO:
- ~~make 404 a thing cause otherwise it crashes the server cause of favicon.ico~~ Fixed :)
- ~~make / redirect to index.html *properly*~~ it should *always* work now i hope
- ~~add threading cause it would be cool and would help performance~ Done, this helped images work and closing the client socket also helped images load as well
- fix the server segfaulting occasioanally, (thank you C++)
- add extra HTTP headers and parse the HTTP headers from the request better
- add POST requests, and make a little API with it :D

:)
