using namespace std;

#include <iostream>
#include "boardrep.h"
#include "engine.h"

int main()
{
	
	cout << "setup" << endl;
	
	State s;
	s.setup();
	s.drawascii();
	
	cin.ignore();
	//cout << "setting square 22 to values" << endl;
	
	//for(int v=0; v<=4; v++)
	//{
		//s.setCell(22,v);
		//s.drawascii();
		//cin.ignore();
	//}
	
	
	
	cout << "setup again." << endl;
	s.setup();
	s.drawascii();
	cin.ignore();
	
	cout << "setting progressive squares to white king" << endl;
	
	for(int i=0; i<32; i++)
	{
		s.setup();
		s.setCell(i,WQUEEN);
		s.drawascii();
		cin.ignore();
	}
	
	
	//cout << "flipped." << endl;
	//s.flip();
	//s.drawascii();
	//cin.ignore();
	
	for(int i=0; i<10; i++)
	{
	cout << "random setup " << i << endl;
	s.random();
	s.drawascii();
	
	cout << "sum of white men: " << bboardSum(s.data.PMASK & (~s.data.BMASK) & (~s.data.KMASK)) << endl;
	cout << "sum of black men: " << bboardSum(s.data.PMASK & s.data.BMASK & (~s.data.KMASK)) << endl;
	cout << "valutation: " << s.value_function() << endl;
	cin.ignore();
	}
	
	
	for(int i=0; i<10; i++)
	{
	cout << "random setup " << i << endl;
	s.random();
	s.drawascii();
	cout << "flip" << endl;
	s.flip();
	s.drawascii();
	cin.ignore();
	}
	
	
	cout << "left rotation" << endl;
	s.random();
	s.drawascii();
	s.data.PMASK = ROT1(s.data.PMASK);
	s.drawascii();
	cout << "anti-right rotation" << endl;
	s.random();
	s.drawascii();
	s.data.PMASK = ANTIROT2(s.data.PMASK);
	s.drawascii();
	
	cin.ignore();
	
	for(int i=0; i<10; i++)
	{
	
			cout << "counting jumpers " << i << endl;
			
			s.random();
			s.drawascii();
			s.data.PMASK = s.data.jumpers();
			s.drawascii();
	}
	
}
