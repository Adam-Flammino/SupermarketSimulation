// SupermarketSimulation.cpp
// Adam Flammino 8/18/17
// Simulates customer wait time with varying numbers of customers and casheirs

#include "stdafx.h"


// Customers in line
class Customer
{
private:
	int timeAtRegister;
	int waitingSince;
public:
	// Constructor, takes time customer started waiting
	Customer(int t)
	{
		waitingSince = t;
		timeAtRegister = rand() % 4; // Each customer takes 1-3 minutes checking out
	}
	// Lets a minute pass. Returns true if customer is done checking out, false if not
	bool doneYet()
	{
		timeAtRegister--;
		bool flag = (timeAtRegister == 0);
		return flag;
	}
	// Gets time customer has been waiting since
	int getWaitTime()
	{
		return waitingSince;
	}
};

// Cashiers, both actively working and not
class Cashier
{
private:
	bool working; // Is cashier currently working the register
	bool available; // True = no customer, false = customer
	int line = 0;
public:
	// Default constructor
	Cashier()
	{
		working = false;
		available = true;
	}
	Cashier(bool start)
	{
		working = start;
		available = true;
	}
	// Goes off register when no customers to help
	void setWorking(bool b)
	{
		working = b;
	}
	// Check if available
	bool getAvailable()
	{
		if (line > 0)
		{
			available = false;
		}
		else
		{
			available = true;
		}
		return available;
	}
	// Check if working
	bool getWorking()
	{
		return working;
	}
	// Increment line
	void newCust()
	{
		line++;
	}
	// Decrement line
	void finishedCust()
	{
		line--;
	}
	// Get line size
	int getLine()
	{
		return line;
	}
};

// Selects a line for customer
void lineSelection(int absoluteTime, Cashier *cashiers, std::deque<Customer> *lines)
{
	bool working;
	bool available;
	bool waiting = true; // Tracks whether each customer is still waiting
	int shortestLine = INT_MAX; // For selecting shortest line when all are filled
	int lineToPick;
	for (int i = 0; i < 9; i++) // Checks for a cashier that is working and available, goes to that line
	{
		working = cashiers[i].getWorking();
		if (working)
		{
			available = cashiers[i].getAvailable();
			if (available)
			{
				lines[i].push_back(Customer(absoluteTime));
				waiting = false;
				cashiers[i].newCust();
			}
		}
	}
	if (waiting) // If there are no available cashiers
	{
		for (int i = 0; i <= 6; i++) // Look for lines of 6 of less
		{
			for (int j = 0; j < 9; j++) { // Check each cashier
				working = cashiers[j].getWorking();
				if (working && waiting)
				{
					if (lines[j].size() < i) // Look for shortest open line
					{
						lines[j].push_back(Customer(absoluteTime));
						cashiers[j].newCust();
						waiting = false;
						break;
					}
				}
				else if (!waiting)
				{
					break;
				}
			}
		}
	}
	if (waiting) // If all working cashiers have 6 customers
	{
		for (int i = 1; i < 9; i++) // Put new customer in next cashier that isn't working
		{
			working = cashiers[i].getWorking();
			if (!working)
			{
				cashiers[i].setWorking(true);
				cashiers[i].newCust();
				lines[i].push_back(Customer(absoluteTime));
				waiting = false;
				break;
			}
		}
	}
	if (waiting) // If all cashiers are currently working and have 6+ customers, go to shortest line
	{
		for(int i = 0; i < 9; i++)
		{
			working = cashiers[i].getWorking();
			if (working)
			{
				if(lines[i].size() < shortestLine)
				{
					shortestLine = lines[i].size();
					lineToPick = i;
				}
			}
		}
		lines[lineToPick].push_back(Customer(absoluteTime));
		cashiers[lineToPick].newCust();
	}
}
int main()
{
	int hour = 3;
	int minute = 0;
	int waitTime;
	int workingCashiers;
	int absoluteTime; // Holds both hours and minutes to see how long a customer has been waiting
	int lineSize;
	Cashier cashiers[9];
	std::deque<Customer> lines[9];
	cashiers[0] = Cashier(true);
	bool working;
	bool available;
	bool custDone;
	int pause;
	std::cout << "How many milliseconds would you like to pause between each miniute?\n";
	std::cin >> pause;
	for (hour; hour < 7; minute++)
	{
		if (minute == 60) // Advance hour every 60 minutes
		{
			hour++;
			minute = 0;
		}
		absoluteTime = (hour * 60) + minute;
		if (hour > 5 && hour < 8) // Peak time
		{
			if (rand() % 11 > 1) // 90% chance of a customer coming in each minute during peak time
			{
				lineSelection(absoluteTime, cashiers, lines);
			}
		}
		else // Off-peak
		{
			if (rand() % 11 > 8) // 20% chance of a customer coming in each minute during off-peak
			{
				lineSelection(absoluteTime, cashiers, lines);
			}
		}
		for (int i = 1; i < 9; i++) // If more than 1 cashier is open, make sure all cashiers have customers, otherwise close those lines
		{
			working = cashiers[i].getWorking();
			if (working)
			{
				available = cashiers[i].getAvailable();
				if (available)
				{
					cashiers[i].setWorking(false);
				}
			}
		}
		workingCashiers = 0;
		lineSize = 0;
		for(int i = 0; i < 9; i++) // Counts how many cashiers are working
		{
			working = cashiers[i].getWorking();
			if (working)
			{
				workingCashiers++;
				if (lineSize < cashiers[i].getLine()) {
					lineSize = cashiers[i].getLine();
				}
			}
		}
		std::cout << "It is currently " << hour << ":" << std::setw(2) << std::setfill('0') << minute <<
			" and there is/are " << workingCashiers << " open registers\n\n";
		if (lineSize != 0) // If there is a line..
		{
			std::cout << "The last person in each line has been waiting/checking out for:\n";
			for (int i = 0; i < workingCashiers; i++)
			{
				waitTime = (absoluteTime - lines[i].back().getWaitTime());
				std::cout << "Line " << i + 1 << " : " << waitTime << " minutes\n";
			}
			for (int i = 0; i < workingCashiers; i++) // Checks if customer in front of each line is done, removes them from line if they are
			{
				custDone = lines[i].front().doneYet();
				if (custDone)
				{
					lines[i].pop_front();
					cashiers[i].finishedCust();
				}
			}	
		}
		else
		{
			std::cout << "There is no one in line.\n\n\n";
		}
		std::cout << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(pause)); // Pause for pause milliseconds after each loop
	}
return 0;
}

