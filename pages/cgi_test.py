#!/usr/bin/python
# Import modules for CGI handling 
import sys, ctypes

# Get data from fields

num1 = int(sys.argv[1]);
num2  = int(sys.argv[2]);
result = num1 + num2;

print "Content-type:text/html\r\n\r\n"
print "<html>"
print "<head>"
print "<title>CGI Program</title>"
print "</head>"
print "<body>"
print "<h2>Result: %d + %d = %d</h2>" % (num1, num2, result)
print "</body>"
print "</html>"