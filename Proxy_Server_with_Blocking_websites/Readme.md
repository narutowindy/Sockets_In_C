A2
A2: The second part

In this part you will extend the stage 1 program to become a full web proxy, a program that, as discussed in class, reads a request from a browser, forwards that request to a web server, reads the reply from the web server, and forwards the reply back to the browser. Your proxy need only handle a single connection at a time. It should accept a new connection from a browser, completely handle the request and response for that browser, and then start work on the next connection. (A real web proxy would be able to handle many connections concurrently.) You will then add the ‘blocking’ part which means web requests to black listed web sites will not be returned.

Design Requirements

Your proxy will speak a subset of the HTTP/1.1 protocol, which is defined in RFC 2616. You're only responsible for a small subset of HTTP/1.1. Your proxy should satisfy these requirements:

Your proxy should emit (to standard output) a startup line as well as ONE log line for each request it processes or blocks. (See the next section for examples.)
GET requests work and images and arbitrary length binary files are transferred correctly.
Your proxy should properly handle Full-Requests (RFC 1945 section 4.1) up to 65535 bytes. You may close the connection if a Full-Request is larger than that.
You must support URLs with a numerical IP address instead of the server name (e.g. http://128.181.0.31/).
You are not allowed to use fork() or threads/multiprocesses of any kind.
You may not allocate more than 30MB of memory or have more than 32 file descriptors.
Your proxy should correctly service each request if possible. If errors occur it should close the connection and then proceed to the next request. If an error occurs from which the proxy cannot reasonably recover, the proxy should print an error message on the standard output and call exit(1). There are not many non-recoverable errors; perhaps the only ones are failure of the initial socket(), bind(), listen() calls, or a call to accept(). Your proxy should never dump core except in situations beyond your control (e.g. hardware/OS failure) and your proxy must not crash for any kind of signal errors other than hardware failures.
A list of ‘blocked’ web sites will be supplied: requests to these sites should not be fulfilled, instead an error page will be returned.
You may NOT use any predefined libraries (e.g. libwww, libhttpX) or parsers or copy others’ code or code from the web. If you are in doubt ask. Using libraries that have not been cleared with the instructor will result in significant grade deductions.
Your proxy should be written in at most 300 lines of C or Java (excluding comments) and it should compile and run on AFS. No other languages or libraries are permitted.
Java projects will be submitted as a SINGLE file (Web.java) which will be compiled with the command line Java compiler (javac Web.java) on AFS. We will run this as a standalone Java program from the command line (java Web 8877 >out.txt). You can develop it wherever you like, but when you submit, it should be an independent Java class that can compile on its own, outside of an IDE or any additional libraries. If you have questions, please ASK.

Note:
In addition to the doParse() and doLookup() functions (or methods) you will add another method called doRequest() which will be called after doLookup() and which will do the actual retrieval and proxying.
Also note the NEW programming rules:

In Java you cannot use any of the following: String (and friends like StringReader, StringBuffer, StringTokenizer etc), Character (or similar higher order data wrapper classes), BufferedReader/Writer, PrintReader/Writer and the like. The only exception is the usage of a String in a constructor (as an unnamed variable) to pass to another constructor. (Very limited latitude, so ASK if you have questions.)
In Java, reading and writing from a Socket (Java) must use only InputStream.read(byte [], 0, 0) and OutputStream.write(byte [],0,0); no other methods to read/write are permitted. This means when you receive the request from a browser (in a byte []) then you must use the same byte array to parse/handle the request. In C you must use read(2) and write(2) system calls.
In ServerSocket you must use the ServerSocket() constructor only, and you must use 
bind (InetSocketAddress)before you accept(). (You cannot directly accept().) In C you must follow the normal sequence i.e. socket(), bind(), connect() and so on.
In Java DNS lookup must use InetAddress.getAllByName(String S) and you must write the logic (code) to select the appropriate InetAddress from the array, to be used in the ServerSocket.bind() call. In C you must use getaddrinfo(3).
You cannot use StringTokenizer OR friends (Java) and you cannot use strtok() in C. You must scan the URL manually.
Java Exceptions must be caught and printStackTrace() must be used. You cannot evade or ignore Exceptions; try to handle them as well as you can. I reserve the right to add or modify Exception handling, and poor Exception handling will result in poor grades. Specifically, Java code that catches a blanket Exception object (thus smothering all kinds of errors) will result in grade deductions.
The standard University Academic Integrity Policy applies. Violations will result in a grade of zero for the project.
You are welcome to discuss your project with other groups or seniors, but ALL your code should be your group’s own work. You cannot copy code from the UNP book, from the net or from any other sources. Note that your code will first be run through MOSS which checks it automatically, line for line, against all previously submitted projects as well as a dozen or so projects in Github. If you are caught then your code will not be run and your group will get an automatic zero on the project. You will be referred to the Dean’s office.
You do not have to worry about POST, non HTTP URL/s or HTTP-headers (RFC section 4.2) for now.

Running and testing your proxy
This will be the same as before:

$ ./stage2 2000                       ## listens on port 2000
stage 2 program by (group-leader-UCID) listening on port (2000)
REQUEST: http://qprober.cs.columbia.edu/publications/...  ## URL Requested
Ctl-C                                 ## how we’ll stop it

Note carefully the startup line (printed once on startup to standard out) which should indicate the group number and the group leader’s email. All requests should be logged with a REQUEST line on standard output.

Submit / Deliverables

Each team should submit a single file (web.c or Web.java) that will be compiled and run as described above. We will test using a dozen or so requests. Tests will be done by using the browser proxy settings. Roughly speaking programs that pass all tests will receive full credit, and partial tests will receive partial credit on a sliding scale. If your program crashes then you will get ZERO – so be careful to test your program on all the error cases or corner cases you can think of.

Grading / Testing

We will test your proxy using one or more of the following tests:

Ordinary fetch            Fetch a normal web page.
Split request               Request in more than one ‘chunk’ (connection)
Large request/resp      Requests of size 65535 bytes or response of size about 200MB
Bad Connect               URL to a bad port or bad host (e.g. blahblah.com:2222)
Malformed request     HTTP request that is syntactically incorrect
Huge request               Requests larger than 65535 bytes (of up to 1MB)
Stress test                   We will test your proxy with several high-speed requests

We may use some combination of these tests, including some unspecified tests that are within HTTP protocol. (Think of this as a demo to your ‘startup’ venture capitalist for next round funding. Just as at a startup, you will have to design your own tests to mirror how we will test your code.) Your final grade will depend on your design and the performance of your code. Roughly speaking, proxies that pass our tests and remain standing will earn full marks, each failed test gets a 5-10% deduction, proxies that don’t even do Ordinary Fetch will get zero.

Note that your design plays an important part of your grade. Your program or class must have at least five to six distinct functions or methods which should handle the various parts of the job. Your main function (or run() method) should call these functions and the code should be readable. To ensure good design I will review your code structure during office hours (roughly week of TBD). Poorly designed projects, even if they work perfectly, may be penalized up to 50% of overall project grade.

Happy programming!

KXM
