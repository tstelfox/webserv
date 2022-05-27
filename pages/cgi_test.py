#!/usr/bin/python
# Import modules for CGI handling 
import sys, ctypes

# Get data from fields

num1 = int(sys.argv[1]);
num2  = int(sys.argv[2]);
result = num1 + num2;

print "HTTP/1.1 200 OK\nServer: Flamenco FLame Server"
print "Content-type: text/html; charset=UTF-8\nContent-Length:",
htmlBody = "<html>\n\t<head>\n\t\t<title>CGI Program</title>\n\t</head>\n\t<body>\n\t\t<h2>Result: %d + %d = %d</h2>" % (num1, num2, result) + \
        "\n\t</body>\n</html>"
print len(htmlBody),
print "\r\n\r\n",
# print ("The size is: ")
print (htmlBody)
# print "<html>"
# print "<head>"
# print "<title>CGI Program</title>"
# print "</head>"
# print "<body>"
# print "<h2>Result: %d + %d = %d</h2>" % (num1, num2, result)
# print "</body>"
# print "</html>"