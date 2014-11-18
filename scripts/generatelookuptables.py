#Questo script genera le lookup tables per le mosse

def findstep(n):
	if n in range(4):
		return (99,99)
	if n in (4,12,20,28):
		return (n-4,99)
	if n in (11,19,27):
		return (n-4,99)
	if (n//4)%2 == 0:
		return (n-4,n-3)
	else:
		return (n-5,n-4)
def flipn(n):
	if n==99:
		return n
	else:
		return 31-n

def flip (t):
	return tuple(( flipn(l) for l in t))

def findstepking(n):
	return findstep(n) + flip(findstep(31-n))

for i in range(32):
	(a,b) = findstep(i)
	print "{%d,%d}"%(a,b) + ", //%d"%i

for i in range(32):
	(a,b,c,d) = findstepking(i)
	print "{%d,%d,%d,%d}"%(a,b,c,d) + ", //%d"%i


def ntoxy(n):	

	x= 2*(n%4) + 1 - ((n//4)%2)

	y = n//4

	return (x,y)

def xyton(p):
	(x,y)=p
	if((x<0) or (x>=8) or (y<0) or (y>=8)):
		return 99
	n = 4*y + x/2
	return n


print "hey ",ntoxy(26),ntoxy(14)
print xyton(ntoxy(26)),xyton(ntoxy(14))


def findjump(n):
	(x,y) = ntoxy(n)
	l = [xyton((x-2,y-2)),xyton((x+2,y-2)),xyton((x-2,y+2)),xyton((x+2,y+2))]
	
	return l

for i in range(32):
	
	print "{%d,%d,%d,%d}"%tuple(findjump(i)) + ",\t //%d"%i
