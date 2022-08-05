import cgi
import cgitb; cgitb.enable() # Optional; for debugging only

print "Content-Type: text/html"
print ""

arguments = cgi.FieldStorage()
for i in arguments.keys():
 print arguments[i].value
