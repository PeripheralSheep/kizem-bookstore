/*This program is designed to keep book inventory and customer accounts, 
allow users to search and purchase books and allows admins to see the sales data*/ 
//-------------------LIBRARIES--------------------------------------------//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <windows.h>
#include <time.h>

//----------------------CONSTANTS-----------------------------------------//
#define DISCOUNTVALUE 0.05		//The value of the discount in decimal form
#define MONEYFORDISCOUNT 100	//constant used to determine how much money is needed for a discount

//----------------------GLOBAL VARIABLES----------------------------------//
float sessiontotal=0;							//holds the total money made during an executable's time running
float lifetimetotal; 							//holds the total money made since the inception of the e-store
float sessiondelivery = 0, lifetimedelivery;	//holds the amount of money made from $5 deliveries both since inception and the executable's time running
int soldbooks_session=0,soldbooks_total;		//holds the number of units sold during an executable's time running and overall
char adminpassword[] = {"Access"};				//Contains the password used to access information for admins
int numcustomers=0;								//holds the number of customer records currently in use
int numbooks=0;									//holds the number of different books the store has


//------------------------STRUCTURES----------------------------------------//

//The structure below is used to store customer account details from the textfile as well as from the customer creation process
struct customerdetails
	{
		char firstname[16],lastname[16];	//holds user's first name and last name
		char address[51];					//holds the user's addresses
		char username[31],password[31];     //holds the username and password that the user must use to log in to purchase
	};
struct customerdetails datastore[100];  	//creates an structure array that holds the customer details of all customers
struct customerdetails loadeddata;      	//creates a single instance that holds the customer details of the logged in user

//The structure below stores the information of all the books in the textfile
struct items
	{
		char bookname[21];	//holds the name of the book
		char genre[31];		//holds the genre that the book is classified as
		char ID[6];			//holds the unique ID of the book
		char author[31]; 	//holds the author who created the book
		float price;		//holds the price of the book
	};
struct items books[100];	//creates a structure array that holds the information of up to a maximum of 100 books


/*The structure below stores the information of what is bought during the 
purchasing function and displays it as a receipt at the end of the function*/
struct receipt				
	{
		char itemname[31];		//holds the name of the book on the receipt
		float unitprice; 		//holds the price of 1 instance of the item
		int quantity;			//holds the amount of the book wanted to be purchases
		float subtotals;     	//used to store the result of unitprice * quantity
	};
struct receipt purchases[100];	//creates a structure array to hold up to 100 items on the receipt

//----------------------FUNCTIONS----------------------------------//

//All functions are forward declared so that the program knows of each functions existence
void logo(int n);
void startingmenu();
void converter(char variable[],char startingsymbol,char targetsymbol);
int exitprematurely();
int customermenu(struct customerdetails creations[]);
int customercreation(struct customerdetails creations[]);
int customerlogin(struct customerdetails creations[]);
int purchasing(struct items booklist[],struct customerdetails logged,struct receipt customers[]);
int customerlogout(struct customerdetails logged);
int salesdata();
int search(struct items booklist[]);

//----------------------------------------------------LOGO-----------------------------------------------------------------//

/*The function below prints the business' Signature K Logo on entry and on exit.*/
void logo(int n)
	{
		printf("\n\n");
		char half[]= {"       OOOOOOOOO"};   //used to create the non-stem part of the K
		int loop=8; 						 //variable which is utilised in for loops for stop condition and starting value
		int i;                               //For loop variable control

		//The for loop below prints the upper half of the K
		for (i=0; i < loop; i++)
			{
				printf("                            OOOOOOOOO");	
				printf("%s\n", &half[i]); //Causes the string to print from a specific index location
				Sleep(200);
			}
		//The for loop below prints the lower half of the K
		for (i=loop-1; i>= 0; i--)	
			{
				printf("                            OOOOOOOOO");
			    printf("%s\n", &half[i]); //Causes the string to print from a specific index location
				Sleep(200);
			}
		//The value of n is based on if the logo is being used for the welcome or exit
		if(n == 1)
			printf("\n                            W   E   L   C   O   M   E   ");
		else
			printf("\n                     Thank you for shopping, have a nice day!\n\n");
	}
	
	
//----------------------------------------------------STARTING MENU-----------------------------------------------------------------

/*The starting menu function welcomes users and displays the list of options that users can choose from by entering the number of the option*/
void startingmenu()
	{
		char namewithcomma[17]={"\0"};					//used to hold the customers name with a comma included
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);	//Handle needed to change colour console colour
		SetConsoleTextAttribute(hConsole, 11);              //Changes the colour of the console text to turquoise
		printf("\t\t===========================================================================\n");
		printf("\t\t===========================================================================\n");
		printf("\t\t===========              KIZEM'S ONLINE BOOKSTORE               ===========\n");
		printf("\t\t===========================================================================\n");
		Sleep(750);
		printf("\t\t===========                  Welcome ");
		
		//If statement below joins a user's firstname and a comma without messing up formatting
		if(strcasecmp(loadeddata.username,"\0")!=0)	
			{
				strcat(namewithcomma,loadeddata.firstname);
				strcat(namewithcomma,",");
				printf("%-17s",namewithcomma);		
			}				
		else
			printf("%-17s","customer,");		
		printf("          ===========\n");
		printf("\t\t===========    Please select the number of your choice below    ===========\n");
		printf("\t\t===========================================================================\n");
		Sleep(1000);		
		printf("\t\t===========                                                     ===========\n");
		
		if(strcasecmp(loadeddata.username,"\0")==0)
			printf("\t\t===========           1. Customer Creation and Login            ===========\n");
		else
			printf("\t\t===========           1. Log Out                                ===========\n");
			
			printf("\t\t===========                                                     ===========\n");
			printf("\t\t===========           2. Search for books                       ===========\n");
			printf("\t\t===========                                                     ===========\n");
			printf("\t\t===========           3. Purchase Books                         ===========\n");
			printf("\t\t===========                                                     ===========\n");
			printf("\t\t===========           4. Access Sales Data [Admin Only]         ===========\n");
			printf("\t\t===========                                                     ===========\n");
			printf("\t\t===========           5. Exit Program                           ===========\n");
			printf("\t\t===========                                                     ===========\n");
			printf("\t\t===========================================================================\n");
			printf("\t\t===========================================================================\n\n");
			printf("\t\tSelection: ");		
	}
//---------------------------------------------------END OF FUNCTION----------------------------------------------------------//

//----------------------------------------------------Converter---------------------------------------------------------------//

/*This function will be used to convert symbols in strings to another symbol
  Namely for the use of replacing underscores from textfiles to spaces when read to the program
  and to replace the spaces created in the program with underscores when writing back to the program*/
void converter(char variable[],char startingsymbol,char targetsymbol)
	{
		int x; //controls for loops within the function
		
		for(x=0;x<strlen(variable);x++) //the for loop is as long as the word that replaces the dummy variable
			{
				if(variable[x] == startingsymbol)       //Checks to see if the character array location of the variable matches the starting symbol
					variable[x]= targetsymbol;			//Converts the location to the specified target symbol
			}
	}
//---------------------------------------------------END OF FUNCTION----------------------------------------------------------//


//----------------------------------------------------EmergencyExit-----------------------------------------------------------//
//The function below is implemented within other functions to allow them to exit the function prematurely
int exitprematurely()
	{
		char choice; //holds the users input for decisions
		printf("\t\tWould you like to:\n\t\t1. Continue\n\t\t2. Go back\n\n\t\tSelection: ");
		scanf(" %c",&choice);
	
		while(choice!= '1'&&choice!='2')  //error check to see if user does not input one of the two options
			{
				printf("\n\t\tPlease enter:\n\n\t\t'1' to continue\n\t\t'2' to go back\n\n\t\tSelection: ");
				scanf(" %c",&choice);
			}	
		if(choice=='1') 
			return 1;			//if 1 is returned, the function will continue
		else if(choice=='2')
			{
				printf("\n\t\tReturning.\n\n");
				system("pause");
				return 2;		//if 2 is returned, the function is discontinued
			}				
	}
//---------------------------------------------------END OF FUNCTION----------------------------------------------------------//


//----------------------------------------------------CustomerMenu------------------------------------------------------------//
//The function below contains the menu that will displays options that deal with the creation and logging in of customer accounts
int customermenu(struct customerdetails creations[])
	{
		char choice;										//holds the user's input for decisions
		int tomainmenu;										//Variable used to determine if user is sent directly to the main menu after logging in	
	
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);	//Handle needed to change colour console colour
		do
			{
				system("CLS");
				SetConsoleTextAttribute(hConsole, 9); 		//changes console text colour to blue
				printf("\n\n\n\t\t===========================================================================\n");
				printf("\t\t===========================================================================\n");
				printf("\t\t===========                    CUSTOMER MENU                    ===========\n");
				printf("\t\t===========================================================================\n");
				printf("\t\t===========                  Welcome ");
				printf("%-17s","customer,");		
				printf("          ===========\n");
				printf("\t\t===========    Please select the number of your choice below    ===========\n");
				printf("\t\t===========================================================================\n");
				Sleep(750);
				printf("\t\t===========                                                     ===========\n");
				printf("\t\t===========                1. Create an Account                 ===========\n");
				printf("\t\t===========                                                     ===========\n");
				printf("\t\t===========                2. Log in                            ===========\n");
				printf("\t\t===========                                                     ===========\n");
				printf("\t\t===========                3. Return to Main Menu               ===========\n");
				printf("\t\t===========                                                     ===========\n");
				printf("\t\t===========================================================================\n");
				printf("\t\t===========================================================================\n\n");
				printf("\t\tSelection: ");		
				scanf(" %c",&choice);
				
				while(choice != '1'&&choice!='2'&&choice!='3') 				   //error check for incorrect input
					{
						printf("\n\t\tEnter '1' to Create an Account, enter '2' to Log in\n\t\tEnter '3' to Return to Main Menu\n\n\t\tSelection:");
						scanf(" %c",&choice);
					}
				
				switch(choice)
					{
						case '1':
							{
								customercreation(creations); 				   //sends the user to create an account
								break;
							}
						case '2':
							{
								if((tomainmenu= customerlogin(creations))==1) //sends the user to log in
									return 0;								  //If successful, the user returns immediately 	
								break;										  //To main menu as the customer menu no longer has a purpose
							}
						
						case '3':
							{
								printf("\n\t\tReturning to main menu\n");	
								system("pause");
								return 0;                                     //returns the user to the main menu
								break;
							}
					} //end of switch case
			}while(choice!='3');
	}
//---------------------------------------------------END OF FUNCTION----------------------------------------------------------//


//----------------------------------------------------CustomerCreation--------------------------------------------------------//
//The function allows users to create an account from which they can purchase with	
int customercreation(struct customerdetails creations[])
	{
		int  exitdecision;	//holds the return value from the exitprematurely function to decide if the function is aborted prematurely	
		char firstname[16]; //holds what the user wants as their first name
		char lastname[16];  //holds what the user wants as their last name
		char address[51];	//holds what the user wants as their address
		char username[31];	//holds the username the user would like to use
		char password[31];	//holds the password the user would like to use
		int x;				//The variable that controls for loops
		int invalid=0;      //variable that lets the program know if the input was acceptable
		char retry;			//Variable that the user inputs to if they want to retry after entering invalid data
		char confirm;       //Variable that the user inputs to as confirmation of a decision 
		
		system("CLS");
		printf("\n\n\t\tThis function will allow customers to create an account needed to purchase books.\n\n");
		if((exitdecision=exitprematurely())==2) //if the exitprematurely returns 2 to exitdecision, the function is aborted
			return 0;		
		system("pause");
		system("CLS");
		printf("\n\n\t\t\tCustomer Creation\n\n\t\tFirst Name: ");  
		scanf(" %s",&firstname);	//scans the user's input for first name
		printf("\n");
		printf("\t\tLast Name: ");
		scanf(" %s",&lastname);		//scans the user's input for last name
		
		do							//a do..while is employed incase the user enters an invalid/used username
			{
				printf("\n");
				printf("\t\tUsername [No Spaces]: ");	
				scanf(" %[^\n]s",&username);			//scans the user's	for username
				invalid = 0;							//re-intializes the variable back to 0 for further uses
				for(x=0;x<strlen(username);x++)
					{
						if(username[x]==' ')			//If users enter a space in their username, it is considered invalid
							{
								invalid = 1;			//changes invalid to 1 so program knows it is invalid
							}
					}	// end of for loop
				if(invalid == 1)						//Tells the user why it is invalid
					{
						printf("\n\t\tThat is an invalid username. Please ensure to not have any spaces in your username.\n");
						Sleep(1000);
					}   // end of if
											
				for(x=0;x<numcustomers;x++)								   //Searches the customer record database for any reoccurrences of the same username
					{									
						if(strcasecmp(username,creations[x].username)==0) //If any reoccurrences occur, the username is not accepted
							{
								printf("\t\tThis username is already in use.\n");								
								invalid = 1;			                  //Acts as a flag so the program knows it is invalid
								Sleep(1000);
							}	//end of if		
					} //end of for loop
				if(invalid!=0)									  //If user name was invalid, enters an if statement that allows users to retry
					{
						printf("\n\t\tEnter '1' to retry\n\t\tEnter any other key to return to customer menu.\n\n\t\tSelection: ");
						scanf(" %c",&retry);					 //Accepts the user's input whether they want to retry or not
						if(retry != '1')
							{
								printf("\t\tAre you sure you want to return to the customer menu?\n\t\tEnter '1' to confirm, any other key to cancel\n\n\t\tSelection:");
								scanf(" %c",&confirm);           //Double checks to see if the user really wants to exit.
								if(confirm == '1')				 
									{
										printf("\t\tReturning to customer menu.\n");
										system("pause");
										return 0; 				//Cancels the creation process and returns user to customer menu
									} 
								else
									{
										retry = '1';			//if user initially said they would like to exit but they changed their mind, they are allowed to retry
										system("pause");
										system("CLS");
									}	
							}//end of inner if
					} //end of outer if
			}while(retry == '1'&&invalid !=0); //retries the username process
			
		do												//a do..while is employed incase the user enters an invalid password
			{
				printf("\n");
				printf("\t\tPassword [No spaces]: ");
				scanf(" %[^\n]s",&password);					
				invalid = 0;
				for(x=0;x<strlen(password);x++)
					{
						if(password[x]==' ') 			//If users enter a space in their password, it is considered invalid
							{
								invalid = 1;			//changes invalid to 1 so program knows it is invalid
							}
					}//end of for loop
				if(invalid == 1)						//tells the user why it is invalid
					{
						printf("\n\t\tThat is an invalid password. Please ensure to not have any spaces in your password.\n\n");
						printf("\t\tEnter '1' to retry\n\t\tEnter any other key to return to customer menu.\n\t\tSelection: ");
						scanf(" %c",&retry); 		    //Accepts the user's input whether they want to retry or not
						system("pause");
						if(retry != '1')
							{
								
								printf("\t\tAre you sure you want to return to the customer menu?\n\t\tEnter '1' to confirm, any other key to cancel\n\t\tSelection: ");
								scanf(" %c",&confirm);	//Double checks to see if the user really wants to exit.
								if(confirm == '1')	    //Cancels the creation process and returns user to customer menu
									{
										Sleep(1000);   
										printf("\n\t\tReturning to customer menu.\n");
										return 0;
									}
								else
									retry = '1';        //if user initially said they would like to exit but they changed their mind, they are allowed to retry
								system("pause");
								system("CLS");
							}// end of inner if
					}	//end of outer if
			}while(retry=='1'&& invalid != 0); //retries the password process
			
		printf("\n\t\tAddress: "); 
		scanf(" %[^\n]s",&address);			  //accepts user's input for their address
		
		//The code below stores all the information the user just entered into the next free array location
		strcpy(creations[numcustomers].username,username);    
		strcpy(creations[numcustomers].password,password);
		strcpy(creations[numcustomers].address,address);
		strcpy(creations[numcustomers].firstname,firstname);
		strcpy(creations[numcustomers].lastname,lastname);
		numcustomers++; 									//increments the variable to the next free location
		
		FILE *sp = fopen("customerinfo.txt","w");           //opens the customer records file in write mode to insert it into textfile
		for(x=0;x<numcustomers;x++)
			{
				fprintf(sp,"%s %s ",creations[x].firstname,creations[x].lastname);   //writes user first name and last name to textfile
				converter(creations[x].address,' ','_');							 //Replaces the spaces in the address with underscores before it is written to keep amount of columns in textfile consistent																					 
				fprintf(sp,"%s %s %s\n",creations[x].address,creations[x].username,creations[x].password); //Writes the rest of user details to textfile
			}
		fprintf(sp,"-1"); //Adds -1 to textfile which marks the end of the textfile
		fclose(sp);
		
		printf("\n\n");
		system("pause");
		system("CLS");
		printf("\n\n\t\tYour account has been successfully created.\n\n");	//lets user know of successful creation and displays the information
		printf("\t\t\tDetails of your account\n\n\t\tFirst Name: %s\n\t\tLast Name:  %s\n\t\tUsername:   %s\n\t\tPassword:   %s\n\t\tAddress:    %s\n\n",firstname,lastname,username,password,address);
		Sleep(1000);
		printf("\t\tYou will now return to Customer Menu\n\n");
		system("Pause");
		return 0;  //returns user to customer menu
	}
//---------------------------------------------------END OF FUNCTION----------------------------------------------------------//


//----------------------------------------------------CustomerLogin-----------------------------------------------------------//
//The function below allows users to log into an account that has been created	
int customerlogin(struct customerdetails creations[])
	{
		int exitdecision; 		//holds the return value from the exitprematurely function to decide if the function is aborted prematurely	
		char username[31];		//holds the username the user would like to use
		char password[31];		//holds the password the user would like to use
		int found = 0;          //Lets the program know that an account with those details are found
		char choice;			//Variable that the user inputs to if they want to retry after entering invalid data
		char confirm;       	//Variable that the user inputs to as confirmation of a decision 
		int x;					//variable used to control for loop
		
		system("CLS");
		printf("\n\n\t\tThis function will allow customers to log into their account which is needed to purchase books.\n\n");
		if((exitdecision=exitprematurely())==2) //if the exitprematurely returns 2 to exitdecision, the function is aborted
			return 0;
			
		do
			{
				system("PAUSE");
				system("CLS");
				found = 0;      //re-initialized  found to 0 so it can be re-used
				printf("\n\n\t\tCustomer Login\n\n\t\tUsername: ");
				scanf(" %s",&username);   //accepts user's input for username
				printf("\t\tPassword: "); 
				scanf(" %s",&password);	  //accepts user's input for password
				
				/*searches through the customer detail records to check to see if both the username and the password inputted 
				  exactly match one of the records*/
				for(x=0;x<numcustomers;x++)
					{
						if(strcmp(username,creations[x].username)==0 && strcmp(password,creations[x].password)==0)
							{
								printf("\n\t\tLog in Successful.\n\n\t\tReturning to main menu.\n\n");
								
								/*Places the information found in the record into the structure instance the holds the
								information of the account logged into*/
								strcpy(loadeddata.username,creations[x].username);
								strcpy(loadeddata.password,creations[x].password);
								strcpy(loadeddata.firstname,creations[x].firstname);
								strcpy(loadeddata.lastname,creations[x].lastname);
								strcpy(loadeddata.address,creations[x].address);
								found = 1; //lets the program know that the log in was successful
								system("Pause");
								return 1;  //returns 1 to a variable in the customer menu 
							}
					}
					
				if(found != 1)  //If the login in is not successful
					{
						printf("\n\t\tLog in unsuccessful. Credentials do not match any created accounts.\n\n");
						printf("\t\tEnter '1' to retry\n\t\tEnter any other key to return to customer menu\n\n\t\tSelection: ");
						scanf(" %c",&choice); //accepts the user choice if they want to retry or not
						if(choice != '1')
							{
								printf("\n\t\tAre you sure you want to return to customer menu?\n\n\t\tEnter '1' to confirm\n\t\tEnter any other key to cancel\n\n\t\tSelection: ");
								scanf(" %c",&confirm); //Double checks with the user to confirm that they want to exit
								if(confirm == '1')
									{
										printf("\t\tReturning to customer menu\n");
										system("Pause");
										return 0;
									}
								else
									choice = '1';	//if user changes their mind, allows them to retry							
							} // end of inner if
					}// end of outer if
			}while(choice == '1');//End of do while that allows users to retry if unsuccessful 
	}
//---------------------------------------------------END OF FUNCTION----------------------------------------------------------//

	
//----------------------------------------------------CustomerLogout----------------------------------------------------------//
//The function below allows users to sign out of an account if they are logged into one		
int customerlogout(struct customerdetails logged)
	{
		int exitdecision;    //holds the return value from the exitprematurely function to decide if the function is aborted prematurely	
		char confirm;        //Variable that the user inputs to as confirmation of a decision 
	
		system("pause");
		system("CLS");
		
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  //handle that allows console colour to change
		SetConsoleTextAttribute(hConsole, 9);				//changes console text colour to blue
				
		printf("\n\n\t\tThis function will allow customers to log out of their accounts.\n\n");
		if((exitdecision=exitprematurely())==2)   //if the exitprematurely returns 2 to exitdecision, the function is aborted
			return 0;
		system("pause");
		system("CLS");	
		printf("\n\n\t\tAre you sure you want to log out?\n\n\t\tEnter '1' to confirm log out\n\t\tEnter any other Key to cancel\n\n\t\tSelection: ");
		scanf(" %c",&confirm);
		if(confirm == '1')
			{
				/*The account information that was stored in the loadeddata instance is replaced with null characters, resetting the variables*/
				strcpy(loadeddata.username,"\0");
				strcpy(loadeddata.password,"\0");
				strcpy(loadeddata.firstname,"\0");
				strcpy(loadeddata.lastname,"\0");
				strcpy(loadeddata.address,"\0");
				printf("\n\t\tYou have been successfully logged out. Returning to main menu\n");
				system("pause");
				return 0;
			}
		else
			{
				printf("\n\t\tReturning to main menu.\n");
				system("Pause");
				return 0;
			}
	}
//---------------------------------------------------END OF FUNCTION----------------------------------------------------------//


//----------------------------------------------------Purchasing--------------------------------------------------------------//
//The function below allows users to purchases books once they are signed into their account		
int purchasing(struct items booklist[],struct customerdetails logged,struct receipt customers[])
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //handle that allows console colour to change
		SetConsoleTextAttribute(hConsole, 10);             //changes the console text colour to bright green
		
		int exitdecision;             //holds the return value from the exitprematurely function to decide if the function is aborted prematurely	
		char bookchoice[21];		  //holds the user's choice of book to buy
		int x;						  //variable control in for loops
		int found=0;                  //variable flag that lets the program if a book was found or not
		int rcptlocation = 0;         //holds the next free location in the receipt structure array
		int quantity;                 //holds the amount of a specified book that the user wants to purchase
		int verification=0;           //used to count the amountof digits the user's inputted credit card number is to verify it is correct
		float oldtotal;               //if there is a discount, this holds the value of the purchase before the discount
		float total=0;		          //holds the current value of a purchase, after discounts if applicable
		char YN;                      //holds users input if they want to retry processes
		char deliverorpickup;         //allows the program to know whether the user wants the order to be delivered to them of if it will be picked up at the store
		char cancellation;            //variable that holds the user's decision to cancel or not
		char confirmation;            //double checks if the user actually wants to cancel
		char creditcardnumber[12];    //holds the user's input for the credit card number
		
		system("CLS");
		
		//user's must be logged into an account before they can purchase
		//it checks to see if the first name slot of the logged in account is not a null character
		if(strcasecmp(logged.firstname,"\0")==0)
			{
				printf("\n\n\t\tPlease log in to your account to conduct purchases.\n");
				system("pause");
				return 0;
			}
			
		printf("\n\n\t\tThis function will allow customers to purchase books that they can pick up at the store or have delivered to them.\n\n");
		if((exitdecision=exitprematurely())==2)  //if the exitprematurely returns 2 to exitdecision, the function is aborted
			return 0;
		
		do
			{
				printf("\n");		
				do
					{
						system("pause");
						system("CLS");
						found=0;  //re-initializes found to 0 so that it can be reused
						
						
						//the lines below print a book menu that users can look that to make their choices
						printf("\n\n\t\tBook Name\t\t   ID\t   Genre   \t   Price\tAuthor\n");
						printf("\t\t---------\t\t   --\t   -----   \t   -----\t------\n");
						for(x=0;x<numbooks;x++)
							{
								printf("\t\t%-25s %s\t   %-15s $%.2f\t%s\n",booklist[x].bookname,booklist[x].ID,booklist[x].genre,booklist[x].price,booklist[x].author);
							}
						printf("\n\t\tEnter the name or ID of the book you want to purchase\n\t\tEnter '-1' to stop buying books.\n\n\t\tSelection: ");
						scanf(" %[^\n]s",&bookchoice);		
						
						//if users enter -1 to stop buying books
						if(strcasecmp(bookchoice,"-1")==0)
							{
								/*if users enter -1 before buying any books, there isn't a purchase so the program asks
								the user if they would like to cancel the purchase or return to purchasing books*/
								if(rcptlocation==0)
									{
										printf("\n\n\t\tYou have no books added to your cart. \n\n\t\tEnter '1' to return to purchasing books\n\t\tEnter any other key to return to the main menu\n\n\t\tSelection: ");
										scanf(" %c",&YN); 
										if(YN!='1')
											{
												//double checks the user's decision to ensure they want to exit
												printf("\n\t\tEnter '1' to confirm your return to the main menu\n\t\tEnter any other key to add books to your cart\n\n\t\tSelection: ");
												scanf(" %c",&confirmation);
												if(confirmation == '1')
													{
														printf("\n\t\tReturning to main menu.\n\n");
														system("pause");
														return 0;
													}							
												else if(confirmation != '1')
													YN = '1';	//allows users to go back to purchasing books
											}
									} // end of inner if statement
								else
									break; //the user proceeds to checkout
							}	// end of outer if statement 	
						//if the user inputs anything else
						else
							{
								for(x=0;x<numbooks;x++)
									{
										if(strcasecmp(bookchoice,booklist[x].bookname)==0||strcasecmp(bookchoice,booklist[x].ID)==0)
											{
												found=1;	//flags so that the user knows a book has been found and then the book info is displayed.				
												printf("\n\t\tName: %s\t Price: $%.2f\n\n\t\tEnter '1' to confirm that you want this book\n\t\tEnter any other key if not wanted.\n\n\t\tSelection: ",booklist[x].bookname,booklist[x].price);
												scanf(" %c",&confirmation);  //confirms that the user wants this book.
												if(confirmation != '1')
													break;
												else
													{
														printf("\n\t\tHow many would you like?\n\t\tQuantity: ");
														scanf(" %d",&quantity);
														while(quantity <= 0)  //users cannot have a quantity that is less than 0
															{
																//tells the user what they must do to rectify the situation
																printf("\n\t\tPlease enter a quantity greater than 0.\n\t\tQuantity: ");
																scanf(" %d",&quantity);
															}
														//copies the information in the variables to the next free location in the receipt structure
														strcpy(customers[rcptlocation].itemname,booklist[x].bookname);
														customers[rcptlocation].unitprice = booklist[x].price;
														customers[rcptlocation].quantity=quantity;
														customers[rcptlocation].subtotals = customers[rcptlocation].quantity*customers[rcptlocation].unitprice;
														rcptlocation++;  //increments the variable to the next free location
														break;
													}	
											}//end of if that compares input to book records
									}//end of for loop that searches for books that match
								if(found==0) //if the flag remained as 0, the program tells the user a book has not been found
									printf("\n\t\tBook not found. Please ensure you are requesting a book available.\n");
							}// end of else
						
						//if users did not enter -1, they do not have any books in their cart but the choice they entered was not found
						if(rcptlocation==0 && strcasecmp(bookchoice,"-1")!=0)
							{
								printf("\n\t\tDo you want to add your first book to your cart?\n\n\t\tEnter '1' to add your first book\n\t\tEnter any other key to return to the main menu\n\n\t\tSelection: ");
								scanf(" %c",&YN);
								if(YN!='1')
									{
										printf("\n\t\tEnter '1' to confirm your return to the main menu\n\t\tEnter any other key to cancel and add a book to your cart\n\n\t\tSelection:");
										scanf(" %c",&confirmation); //double checks with the user to ensure they want to exit
										if(confirmation == '1')
											{
												printf("\nReturning to main menu.\n\n");
												system("pause");
												return 0;
											}
										else if(confirmation != '1')
											YN= '1'; //allows the user to go back to the purchasing process		
									}
								printf("\n");
							}//end of if statement if users have an empty cart and choice was not found						
						//if users did not enter -1 to cancel and they have books in their cart
						else if(rcptlocation !=0  && strcasecmp(bookchoice,"-1")!=0)
							{
								printf("\n\t\tDo you want to add another book to your cart?\n\n\t\tEnter '1' to add another\n\t\tEnter any other key to proceed to check-out\n\n\t\tSelection: ");
								scanf(" %c",&YN);
								
								if(YN!='1')
									{
										printf("\n\t\tEnter '1' to confirm proceeding to check out\n\t\tEnter any other key to add more books to cart\n\n\t\tSelection: ");
										scanf(" %c",&confirmation); //double checks with the user to ensure they want to exit
										if(confirmation != '1')
											YN = '1';			    //allows the user to go back to the purchasing process			
									}
							}// end of else if				
					}while(YN=='1');//end of do while which adds books to cart until YN does not equal '1
					
				system("Pause");
				system("CLS");
				//displays the user's order and the money required
				printf("\n\n\t\tHere is your order:\n\n\t\t\t\t\tORDER\n\n");
				printf("\t\tQuantity    Book    \t           Unit Price\t   Sub-Total\n");
				printf("\t\t--------    ----    \t           ----------\t   ---------\n");
				for(x=0;x<rcptlocation;x++)
					{
						printf("\t\t   %d\t    %-20s     $%5.2f\t    $%5.2f\n",customers[x].quantity,customers[x].itemname,customers[x].unitprice,customers[x].subtotals);
						total+=customers[x].subtotals;		
					}
				printf("\n\t\tTotal: $%.2f\n\n",total);
				if(total>=MONEYFORDISCOUNT) //if the users purchase is over a certain amount, they are allowed a discount
					{
						printf("\t\tYou are eligible for a %.2f%% discount worth $%.2f\n",DISCOUNTVALUE*100,total*DISCOUNTVALUE);
						oldtotal = total;  				//places the total before discount in a different variable
						total -= total*DISCOUNTVALUE;   //The new total after discount is placed in the variable
						printf("\t\tNew total: $%.2f\n\n",total);
					}
				printf("\t\tWould you like:\n\t\t1. Have the order delivered to you for $5.00\n\t\t2. Pick up the order yourself from the store\n\n\t\tSelection: ");
				scanf(" %c",&deliverorpickup); //lets users decide if they want their orders delivered or they pick it up from the store	
						
				//if the user does not enter a valid input, the process is repeated until they do
				while(deliverorpickup!='1'&& deliverorpickup !='2')
					{
						printf("\n\t\tOnly two options are available:\n\n\t\t1. Have the order delivered to you for $5.00\n\t\t2. Pick up the order yourself from the store\n\n\t\tSelection: ");
						scanf(" %c",&deliverorpickup);
					}
				
				//if the user opts to have the books ordered to him
				if(deliverorpickup=='1')
					{
						total+=5; //the $5 required for delivery is added to the total
						printf("\n\t\tYou are now paying $%.2f + $5.00 delivery fee\n",total-5);					
					}
					
				do
					{
						verification = 0;
						printf("\n\t\tTo complete the transaction, a 10 digit credit card number is required, please enter your credit card number.\n\t\tCredit Card Number: ");
						scanf(" %s",&creditcardnumber);
						
						//checks to see if the user entered 10 digits
						for(x=0;x<strlen(creditcardnumber);x++)
							{
								if(isdigit(creditcardnumber[x]))
									{
										verification++;
									}								
							}
						
						//if there aren't 10 digits or the credit card number is not a length of 10 characters, the program flags it
						if(verification != 10 || strlen(creditcardnumber)!=10)
							{
								printf("\n\t\tThat is not a valid credit card number. You will have to retry the process, otherwise the purchase will be cancelled.\n\n\t\tEnter '1' to retry\n\t\tEnter any other key to exit.\n\n\t\tSelection: ");
								scanf(" %c",&cancellation);  //Takes users input to retry or cancel
								if(cancellation != '1')
									{
										printf("\n\t\tAre you sure you want to cancel?\n\n\t\tPress '1' to Confirm \n\t\tPress any other key to go back\n\n\t\tSelection: ");
										scanf(" %c",&confirmation);  //double checks the user's decision to ensure they want to exit
										if(confirmation == '1')
											{
												printf("\t\tThe purchase has been cancelled. Returning to main menu.\n");
												return 0; //the purchase is cancelled and the user returns to main menu
											} // end of inner if	
									}// end of middle if
							}//end of outermost if
					}while(verification !=10 || strlen(creditcardnumber)!=10); //end of do while for credit card which repeats if both criteria are not met
					
				system("Pause");
				system("CLS");
				//prints the user's receipt
				printf("\n\n\t\t************************************************************\n\n");
				printf("\t\t\t\t  KIZEM'S ONLINE BOOKSTORE\n\n");
				printf("\t\t\t\t\t Receipt\n\n\t\tRecipient: %s %s\n",logged.firstname,logged.lastname); //Adds the user's full name to the receipt 
				printf("\t\t************************************************************\n\n");
				printf("\t\tQty\tBook    \t         Unit Price\t   Sub-Total\n");
				printf("\t\t---\t----    \t         ----------\t   ---------\n");
				
				//goes through all the array locations used in the receipt structure and prints it
				for(x=0;x<rcptlocation;x++)
					{
						printf("\t\t %d\t%-20s       $%5.2f\t    $%5.2f\n",customers[x].quantity,customers[x].itemname,customers[x].unitprice,customers[x].subtotals);	
					}
				printf("\n\t\t************************************************************\n\n");
				if(total>=MONEYFORDISCOUNT) //the program displays information slightly differently if the user received a discount
					{
						//uses the total before the discount to show the discount value and the pre-discount total
						printf("\t\tPre-Discount Total: $%.2f\n",oldtotal);
						printf("\t\tDiscount Value(5%%): $%.2f\n",oldtotal*DISCOUNTVALUE);
						
						//if the user opted for a delivery
						if(deliverorpickup=='1')
							{
								printf("\t\tDiscounted Total:   $%.2f\n",total-5); //total without delivery fee
								printf("\t\tCollection Method:  Delivery\n");
								printf("\t\tDelivery Fee:       $5.00\n");
							}
						else //if the user opted to pick up
							{
								printf("\t\tDiscounted Total:   $%.2f\n",total);
								printf("\t\tCollection Method:  Pick-Up\n");
							}				
						printf("\t\tCredit Card Number: ");				
						//just like an actual receipt, some of the digits are replaced with asterisks so that it's masked
						for(x=0;x<strlen(creditcardnumber);x++)
							{
								if(x<3|| x==9)
										printf("%c",creditcardnumber[x]);
								else 
									printf("*"); 
							}
						printf("\n\n\t\tPAYMENT DUE:        $%.2f\n",total); //shows the total payment inclusive of delivery fee if applicable
						
						//the message shown varies based on whether the user opted for delivery or pick up
						if(deliverorpickup=='2')
							printf("\n\t\tBring this receipt to the store for proof of purchase\n");
						else
							printf("\n\t\tExpect your delivery sometime during the day.\n");
					}// end of most outer if
				else //the user was not eligible for a discount
					{					
						//if the user opted for a delivery	
						if(deliverorpickup=='1')
							{
								printf("\n\t\tTotal:   $%.2f\n",total-5); //total without delivery fee
								printf("\t\tCollection Method:   Delivery\n");
								printf("\t\tDelivery Fee:       $5.00\n");
							}				
						else //if the user opted for pick up
							{
								printf("\n\t\tTotal: $%.2f\n",total);
								printf("\t\tCollection Method: Pick-Up\n");
							}	
						printf("\t\tCredit Card Number: ");
						//just like an actual receipt, some of the digits are replaced with asterisks so that it's masked
						for(x=0;x<strlen(creditcardnumber);x++)
							{
								if(x<3|| x==9)
										printf("%c",creditcardnumber[x]);
								else 
									printf("*");
							}
						printf("\n\n\t\tPAYMENT DUE:        $%.2f\n",total); //shows the total payment inclusive of delivery fee if applicable
						
						//the message shown varies based on whether the user opted for delivery or pick up
						if(deliverorpickup=='2')
							printf("\n\t\tBring this receipt to the store for proof of purchase\n");
						else
							printf("\n\t\tExpect your delivery sometime during the day.\n");
					}// end of most outer else
				sessiontotal+=total;    //adds the total to what was made since the executable was running
				lifetimetotal+=total;   //adds the total to what was made since the program was created
				
				//if the user opted for pickup, 5 dollars are added to the delivery statistics
				if(deliverorpickup == '1')
					{
						sessiondelivery += 5;
						lifetimedelivery += 5;
					}
				
				/*Adds the quantity of the books sold to the amount of books sold since the executable was running 
				and since the program was created*/
				for(x=0;x<rcptlocation;x++)	
					{
						soldbooks_session+=customers[x].quantity;
						soldbooks_total+=customers[x].quantity;	
					}
					
				//opens the admin info textfile for writing and writes the new statistics into it
				FILE *tp = fopen("admininfo.txt","w");
				fprintf(tp,"%.2f %d %.2f",lifetimetotal,soldbooks_total,lifetimedelivery);
				fclose(tp);
				printf("\n\t\t************************************************************\n\n");
				printf("\n\t\t\t  T H A N K  Y O U  F O R  S H O P P I N G\n\n");
				printf("\n\t\t************************************************************\n\n");
						
				//re-initializes the receipt locations back to their original value
				for(x=0;x<rcptlocation;x++)	
					{
						strcpy(customers[rcptlocation].itemname,"\0");
						customers[rcptlocation].unitprice = 0;
						customers[rcptlocation].quantity=0;
						customers[rcptlocation].subtotals = 0;
					}
		
				//variables are re-initialized back to their original value			
				total = 0;
				oldtotal = 0;
				rcptlocation = 0;
				
				system("pause");
				system("CLS");
				printf("\n\n\t\tDo you want to make another purchase?\n\n\t\tEnter '1' to make another purchase\n\t\tEnter any other key to exit\n\n\t\tNumber: ");
				scanf(" %c",&YN); //allows users to make another purchase or exit

				if(YN != '1')
					{
						printf("\n\t\tReturning to main menu\n");
						system("pause");
						return 0;
					}
			
			}while(YN == '1'); //end of most outer do while which repeats if the user wanted to make another purchase
	}
//---------------------------------------------------END OF FUNCTION----------------------------------------------------------//


//----------------------------------------------------Sales Data--------------------------------------------------------------//
//The function below allows administration to check the statistics of their business
int salesdata()
	{
		int exitdecision;   //holds the return value from the exitprematurely function to decide if the function is aborted prematurely	
		int triesleft = 5;  //Gives the user a limited amount of tries to enter the correct password
		char password[16];  //Accept user's input as a possible password
		char choice;        //allows users to retry or abort the function if they entered the wrong password
			
		system("pause");
		system("CLS");
		
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //handle that allows console colour to change
		SetConsoleTextAttribute(hConsole, 2);              //changes the console text colour to dark green
		
		printf("\n\n\t\tThis option will display sales information for the session and since the program's inception.\n\n");
		if((exitdecision=exitprematurely())==2)            //if the exitprematurely returns 2 to exitdecision, the function is aborted
			return 0;
		
		do
			{
				system("Pause");
				system("CLS");
				printf("\n\n\t\tTo gain access to this information, you will need to enter the admin password.\n\n");
				printf("\t\tPassword [%d tries remaining]: ",triesleft);  //shows the user the amount of attempts they have left
				scanf(" %s",&password);
				
				//if the password exactly matches the admin password, access is granted to the statistics
				if(strcmp(password,adminpassword)==0)
					{
						printf("\t\tAccess Granted.\n\n");
						system("pause");
						system("CLS");
						break;
					}				
				else //if they did not enter the correct password
					{
						printf("\n\n\t\tIncorrect password.\n");
						triesleft--;  //decrements the amount of attempts left
						printf("\n\t\tEnter '1' to Retry\n\t\tAny other key to return to main menu\n\n\t\tNumber: ");
						scanf(" %c",&choice); 
						
						//if user opts not to retry
						if(choice != '1')
							{
								printf("\n\n\t\tReturning to Main Menu\n\n");
								system("pause");
								return 0;
							}
						printf("\n\n\t\tRetrying\n");
					}
			}while(triesleft>0); //repeats until the user has not tries left
			
			//if the user keeps trying but exhausts all their tries, the program is exited as an intruder precaution
			if(triesleft == 0)
				{
					printf("\n\n\t\tYou have failed 5 times. Program aborting\n\n");
					system("pause");
					exit(0);
				}
		
			//displays all the statistics of the store once access is granted
			printf("\n\n\t\t\t\tSALES DATA\n\n");
			printf("\t\tSession Total: $%.2f\n",sessiontotal);
			printf("\t\tLifetime Total: $%.2f\n",lifetimetotal);
			printf("\t\tSession Total from Deliveries: $%.2f\n",sessiondelivery);
			printf("\t\tLifetime Total from Deliveries: $%.2f\n",lifetimedelivery);
			printf("\t\tUnits sold for Session: %d\n",soldbooks_session);
			printf("\t\tUnits sold in Lifetime: %d \n\n",soldbooks_total);
			system("pause");
	}
//---------------------------------------------------END OF FUNCTION----------------------------------------------------------//


//----------------------------------------------------Search-----------------------------------------------------------------//
//The function below allows users to access a search menu to check for books before they purchase
int search(struct items booklist[])
	{
		
		int x,y;              //both variables control for loops
		char userchoice[31];  //accepts the user's input choice when searching for books by criteria except by a price range
		char searchchoice;    //accepts the user's input choice when they are selecting which criteria they want to search by
		int exitdecision;     //holds the return value from the exitprematurely function to decide if the function is aborted prematurely	
		int found = 0;        //flag used when user is searching for books to determine if at least one book matching the criteria was found
		int found1 = 0;       //flag used when the program is searching for unique genres or authors to output to the user so they can choose
		char repeat;          //allows users to repeat search methods
		float uprange;		  //holds the upper value of the price range that the user wants to search between
		float lowrange;       //holds the lower value of the price range that the user wants to search between
		float switcher;       //if the lower ranger is greater than the upper range, this variable is used as a temporary storage to switch them
		
			
		do
			{	
				char customercomma[17]={"\0"};
				system("CLS");
				HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  //handle that allows console colour to change
				SetConsoleTextAttribute(hConsole, 14);              //changes the console text colour to beige
				printf("\n\n\n\t\t=============================================================================\n");
				printf("\t\t=============================================================================\n");
				printf("\t\t===========                      SEARCH MENU                      ===========\n");
				printf("\t\t=============================================================================\n");
				printf("\t\t===========                    Welcome ");
				
				//allows the program to display the user's name with a comma without destroying the formatting
				if(strcasecmp(loadeddata.username,"\0")!=0)	
					{
						strcat(customercomma,loadeddata.firstname);
						strcat(customercomma,",");
						printf("%-17s",customercomma);	
					}		
				else
					printf("%-17s","customer,");		
				printf("          ===========\n");
				printf("\t\t=========== Please select the number of your search choice below  ===========\n");
				printf("\t\t=============================================================================\n");
				Sleep(750);
				printf("\t\t===========                                                       ===========\n");
				printf("\t\t===========                  1. Book ID                           ===========\n");
				printf("\t\t===========                                                       ===========\n");
				printf("\t\t===========                  2. Book Name                         ===========\n");
				printf("\t\t===========                                                       ===========\n");
				printf("\t\t===========                  3. Genre                             ===========\n");
				printf("\t\t===========                                                       ===========\n");
				printf("\t\t===========                  4. Author                            ===========\n");
				printf("\t\t===========                                                       ===========\n");
				printf("\t\t===========                  5. Price Range                       ===========\n");
				printf("\t\t===========                                                       ===========\n");
				printf("\t\t===========                  6. Display All Books                 ===========\n");
				printf("\t\t===========                                                       ===========\n");
				printf("\t\t===========                  7. Return to Main Menu               ===========\n");
				printf("\t\t===========                                                       ===========\n");
				printf("\t\t=============================================================================\n");
				printf("\t\t=============================================================================\n\n");
				printf("\t\tSelection: ");		
				scanf(" %c",&searchchoice);
				
				//based on the number the user chooses, different functions are accessed
				switch(searchchoice)
					{
						//users search by ID
						case '1':
							{
								system("pause");
								system("CLS");
								printf("\n\n\t\tThis function will allow you to search for a book using its ID\n\n");
								if((exitdecision = exitprematurely())==2) //holds the return value from the exitprematurely function to decide if the function is aborted prematurely	
									break;
								system("Pause");
								do
									{
										system("CLS");
										found = 0;
										printf("\n\n\t\tEnter four number ID: ");
										scanf(" %[^\n]s",&userchoice);
										system("pause");
										
										//searches the array containing book information to find a book that matches the criteria, in this case ID
										for(x=0;x<numbooks;x++)
											{	
												if(strcasecmp(userchoice,booklist[x].ID)==0)
													{
														printf("\n\t\tBook Found\n\n");
														printf("\t\tID:        %s\n\t\tBook Name: %s\n\t\tGenre:     %s\n\t\tPrice:     $%.2f\n\t\tAuthor:    %s\n",booklist[x].ID,booklist[x].bookname,booklist[x].genre,booklist[x].price,booklist[x].author);
														found = 1; //changes the value of the flag so that the program knows a book was found
													}
											}
											
										//if a book is not found
										if(found == 0)
											{
												system("CLS");
												printf("\n\n\t\tNo books with that ID were found.\n");
											}
											
										printf("\n");		
										system("Pause");
										printf("\t\tWould you like to do another search by ID?\n\n\t\tEnter '1' to repeat\n\t\tEnter any other key to return to menu\n\n\t\tSelection: ");
										scanf(" %c",&repeat); //allows users to repeat the search by ID or to return to main menu
									}while(repeat == '1');    //repeats the do..while if the user requested to repeat
								printf("\n\t\tReturning to search menu\n");
								break;
							} // end of case 1
							
						//users search by book name	
						case '2':
							{
								system("pause");
								system("CLS");
								printf("\n\n\t\tThis function will allow you to search for books by name\n\n");
								if((exitdecision = exitprematurely())==2) //holds the return value from the exitprematurely function to decide if the function is aborted prematurely
									break;
								system("Pause");	
								do
									{
										system("CLS");
										found = 0;
										printf("\n\n\t\tEnter Book Name: ");
										scanf(" %[^\n]s",&userchoice);
										printf("\n\n");
										system("pause");
										system("CLS");
										
										//searches the array containing book information to find a book that matches the criteria, in this case book name
										for(x=0;x<numbooks;x++)
											{	
												if(strcasecmp(userchoice,booklist[x].bookname)==0)
													{
														printf("\n\n\t\tBook Found\n\n");
														printf("\t\tID:        %s\n\t\tBook Name: %s\n\t\tGenre:     %s\n\t\tPrice:     $%.2f\n\t\tAuthor:    %s\n",booklist[x].ID,booklist[x].bookname,booklist[x].genre,booklist[x].price,booklist[x].author);
														found = 1;//changes the value of the flag so that the program knows a book was found
													}
											}// end of for loop
										//if a book is not found
										if(found == 0)	
											printf("\n\n\t\tWe do not have that book.\n");
										printf("\n");
										system("Pause");
										printf("\t\tWould you like to do another search by name?\n\n\t\tEnter '1' to repeat\n\t\tEnter any other key to return to menu\n\n\t\tSelection: ");
										scanf(" %c",&repeat);  //allows users to repeat the search by name or to return to main menu
									}while(repeat == '1');     //repeats the do..while if the user requested to repeat
								printf("\n\t\tReturning to search menu\n");
								break;
							}//end of case 2
					
						//users search by genre	
						case '3':
							{	
								system("pause");
								system("CLS");
								printf("\n\n\t\tThis function will allow you to search for books in a particular genre\n\n");
								if((exitdecision = exitprematurely())==2)  //holds the return value from the exitprematurely function to decide if the function is aborted prematurely
									break;
								system("Pause");
								do
									{
										system("CLS");
										found = 0;
										printf("\n\n\t\tGenres Available:\n\n");
										
										//the nested for loop is used to find unique instances of a type of genre
										//the first for loop searches through the records of the array that holds books information
										for(x=0;x<numbooks;x++)
											{
												found1 = 0;//re-initializes the variable that flags after each repetition of first for loop										
												/*the second for loop searches from the first record to the record directly before the current record of x
												to see if any books have the same genre before the current record*/		
												for(y=0;y<x;y++)
													{
														if(strcasecmp(booklist[x].genre,booklist[y].genre)==0&&x!=0)
															{
																found1 = 1; //flags to tell the program that a book before the current record had the same genre
																break;
															}
													}// end of inner for loop
													
												//if no book before the current record had the same genre as the current record										
												if(found1==0)
													{
														printf("\t\t%s\n",booklist[x].genre);
													}											
											}//end of outer for loop
										printf("\n\t\tEnter Genre: ");
										scanf(" %[^\n]s",&userchoice);
										system("pause");
										system("CLS");
										
										//displays the genre that the user chose
										printf("\n\n\t\t\t\tBooks in %s genre\n\n",userchoice);
										printf("\t\tBook Name\t\t   ID\t   Price\tAuthor\n");
										printf("\t\t---------\t\t   --\t   -----\t------\n");
										
										//goes through the records of the array containing book information
										for(x=0;x<numbooks;x++)
											{
												//if the user's choice matches the genre of the book at an index location
												if(strcasecmp(userchoice,booklist[x].genre)==0)
													{
														printf("\t\t%-25s %s\t   $%.2f\t%s\n",booklist[x].bookname,booklist[x].ID,booklist[x].price,booklist[x].author);
														found = 1; //changes the flag variable so the program knows at least one book was found
													}		
											}
										
										//if no books were found
										if(found==0)
											{
												system("CLS");
												printf("\n\n\t\tNo books were found. Ensure you enter a genre that is present and that it is spelt correctly.\n");
											}
										printf("\n");		
										system("Pause");	
										printf("\t\tWould you like to do another search by genre?\n\n\t\tEnter '1' to repeat\n\t\tEnter any other key to return to menu\n\n\t\tSelection: ");
										scanf(" %c",&repeat);  //allows users to repeat or exit
									}while(repeat == '1');	   //allows the user to repeat the search by genre if they requested
								printf("\n\t\tReturning to search menu\n");
								break;
							}// end of case 3
						//users search by author		
						case '4':
							{
								system("pause");
								system("CLS");
								printf("\n\n\t\tThis function will allow you to search for books made by an author\n\n");
								if((exitdecision = exitprematurely())==2) //holds the return value from the exitprematurely function to decide if the function is aborted prematurely
									break;
								system("Pause");
								do
									{
										system("CLS");
										found = 0;
										printf("\n\n\t\tAuthors Available:\n\n");	
											
										//the nested for loop is used to find unique instances of an author
										//the first for loop searches through the records of the array that holds books information
										for(x=0;x<numbooks;x++)
											{
												found1 = 0;//re-initializes the variable that flags after each repetition of first for loop	
												
												/*the second for loop searches from the first record to the record directly before the current record of x
												to see if any books have the same genre before the current record*/	
												for(y=0;y<x;y++)
													{
														if(strcasecmp(booklist[x].author,booklist[y].author)==0&&x!=0)
															{
																found1 = 1; //flags to tell the program that a book before the current record had the same author
																break;
															}
													}// end of inner for loop
													
												//if no book before the current record had the same author as the current record	
												if(found1==0)
													{
														printf("\t\t%s\n",booklist[x].author);
													}											
											}// end of outer for loop
										printf("\n\t\tEnter Author: ");
										scanf(" %[^\n]s",&userchoice);
										system("pause");
										system("CLS");
										//displays the author that the user chose
										printf("\n\n\t\t\t\tBooks by %s\n\n",userchoice);
										printf("\t\tBook Name\t\t   ID\t   Genre   \t   Price\n");
										printf("\t\t---------\t\t   --\t   -----   \t   -----\n");
										
										//goes through the records of the array containing book information
										for(x=0;x<numbooks;x++)
											{	
												if(strcasecmp(userchoice,booklist[x].author)==0)
													{
														printf("\t\t%-25s %s\t   %-15s $%.2f\n",booklist[x].bookname,booklist[x].ID,booklist[x].genre,booklist[x].price);
														found = 1; //changes the flag variable so the program knows at least one book was found
													}
											}
										
										//if no books were found
										if(found == 0)
											{
												system("CLS");
												printf("\n\n\t\tNo books were found. Ensure you enter an author that is available and that it is spelt correctly.\n");
											}								
										printf("\n");
										system("Pause");
										printf("\t\tWould you like to do another search by author?\n\n\t\tEnter '1' to repeat\n\t\tEnter any other key to return to menu\n\n\t\tSelection: ");
										scanf(" %c",&repeat);  //allows the user to repeat the search by author if they would like to
									}while(repeat == '1');	   //repeats the search by author if the user requested
								printf("\n\t\tReturning to search menu\n");
								break;				
							}// end of case 4
							
						//users search by price range	
						case '5':
							{	
								system("pause");
								system("CLS");
								printf("\n\n\t\tThis function will allow you to search for books in a particular price range\n\n");
								if((exitdecision = exitprematurely())==2)  //holds the return value from the exitprematurely function to decide if the function is aborted prematurely
									break;
								system("Pause");
								do
									{
										system("CLS");
										found = 0;
										printf("\n\n\t\tEnter the maximum price a book can be\n\t\t>$");
										scanf(" %f",&uprange);
										
										//the minimum the value inputted can be is 0, the program allows the user to repeat until they enter a valid value
										while(uprange < 0)
											{
												printf("\t\tThat is not a valid value, please enter a non-negative value \n\t\t>$");
												scanf(" %f",&uprange);
											}
										printf("\n\t\tEnter the minimum price a book can be\n\t\t>$");
										scanf(" %f",&lowrange);
										
										//the minimum the value inputted can be is 0, the program allows the user to repeat until they enter a valid value
										while(lowrange < 0)
											{
												printf("\t\tThat is not a valid price, please re-enter a valid price\n\t\t>$");
												scanf(" %f",&lowrange);
											}
										
										//if the user makes the minimum value greater than the maximum value, the program switches the values
										if(lowrange> uprange)
											{
												printf("\n\t\tYour minimum price is greater than your maximum price. We will switch the values for you.\n");
												switcher = uprange;
												uprange = lowrange;
												lowrange = switcher;		
											}
										system("Pause");
										printf("\n\n");
										system("CLS");
										
										/*if both values are the same, the user is looking 
										for an exact value.	the program accounts for this 
										and gives different outputs based on if the values are the same or not*/								 
										if(lowrange==uprange)
											printf("\n\n\t\t\t\tBooks that are $%.2f\n\n",lowrange);
										else
											printf("\n\n\t\t\t\tBooks Between $%.2f and $%.2f\n\n",lowrange,uprange);
										
										printf("\t\tBook Name\t\t   ID\t   Genre   \t   Price\tAuthor\n");
										printf("\t\t---------\t\t   --\t   -----   \t   -----\t------\n");
										
										//the program searches through all the records of the array holding book information
										for(x=0;x<numbooks;x++)
											{
												//if the book's price is between the ranges, it is printed
												if(booklist[x].price >= lowrange && booklist[x].price<= uprange)
													{
														printf("\t\t%-25s %s\t   %-15s $%.2f\t%s\n",booklist[x].bookname,booklist[x].ID,booklist[x].genre,booklist[x].price,booklist[x].author);
														found = 1; //changes the flag variable so that the program knows at least one book was found
													}
											}
										
										//if no books were found
										if(found == 0)
											{
												system("CLS");
												printf("\n\n\t\tNo books in price range found.\n");											
											}		
										printf("\n");
										system("Pause");							
										printf("\t\tWould you like to do another search by price range?\n\n\t\tEnter '1' to repeat\n\t\tEnter any other key to return to menu\n\n\t\tSelection: ");
										scanf(" %c",&repeat);  //allows the user to repeat the search by price range if they would like to
									}while(repeat == '1');     //repeats the search by price range if requested by the user
									printf("\n\t\tReturning to search menu\n");
								break;
							}// end of case 5
						
						//displays all books
						case '6':
							{
								system("pause");
								system("CLS");
								printf("\n\n\t\tThis function will display every book the store has\n\n");
								if((exitdecision = exitprematurely())==2) //holds the return value from the exitprematurely function to decide if the function is aborted prematurely
									break;
								system("pause");
								system("CLS");
								printf("\n\n\t\tBook Name\t\t   ID\t   Genre   \t   Price\tAuthor\n");
								printf("\t\t---------\t\t   --\t   -----   \t   -----\t------\n");
								
								//displays the information of every book contained in the records
								for(x=0;x<numbooks;x++)
									{
										printf("\t\t%-25s %s\t   %-15s $%.2f\t%s\n",booklist[x].bookname,booklist[x].ID,booklist[x].genre,booklist[x].price,booklist[x].author);
									}
								system("pause");
								break;
							}// end of case 6
						
						//returns the program to the main menu
						case '7':
							{
								printf("\n\t\tReturning to main menu\n");
								system("pause");
								return 0;
							}//end of case 7
						
						//if the user enters a number not within the given options
						default:
							{
								printf("\n\t\tThat is not one of the 7 options. Please enter a valid number\n");
							}// end of default
					}//end of switch 
			}while(searchchoice != '7');//end of most outer do while
	}
///---------------------------------------END OF FUNCTION---------------------------------------------------------------------

//----------------------------------------MAIN FUNCTION---------------------------------------------------------------------//
//This function acts as the core of the program, the reading of textfiles is done here and the functions are implemented here
main()
	{
		char choice;        //variable that accepts the user's input for going to the different aspects of the program
		char confirmation;  //confirmation used when the user requests to exit the program
		
		//opens the three textfiles necessary for this program
		FILE *fp = fopen("itemlist.txt","r");
		FILE *sp = fopen("customerinfo.txt","r");
		FILE *tp = fopen("admininfo.txt","r");

		
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //handle that allows the console colour to change
		SetConsoleTextAttribute(hConsole, 4);              //changes the console text colour to red
		
		//The if statement below terminates the program if key textfiles for the running of the program are not found
		if(!fp||!sp||!tp)
			{
				printf("ERROR: ONE OR MORE NECESSARY TEXTFILES NOT FOUND");
				exit(0);
			}

		//reads the book information contained in the textfile into the structure array meant to hold that information
		fscanf(fp,"%s",&books[numbooks].ID);
		while(strcmp(books[numbooks].ID,"-1")!=0)
			{
				fscanf(fp,"%f %s %s %s",&books[numbooks].price,&books[numbooks].genre,&books[numbooks].bookname,&books[numbooks].author);
				converter(books[numbooks].bookname,'_',' ');
				converter(books[numbooks].author,'_',' ');
				numbooks++;
				fscanf(fp,"%s",&books[numbooks].ID);
			}		
		fclose(fp);
		
		//scans the customer information contained in the textfile into the structure array meant to hold that information
		fscanf(sp," %s",&datastore[numcustomers].firstname);
		while(strcasecmp(datastore[numcustomers].firstname,"-1")!=0)
			{
				fscanf(sp," %s %s %s %s",&datastore[numcustomers].lastname,&datastore[numcustomers].address,&datastore[numcustomers].username,&datastore[numcustomers].password);
				numcustomers++;
				fscanf(sp," %s",&datastore[numcustomers].firstname);
			}
		fclose(sp);
		
		//scans the sales information contained in the textfile into the variables meant to hold that information
		fscanf(tp,"%f %d %f",&lifetimetotal,&soldbooks_total,&lifetimedelivery);
		fclose(tp);
		
		logo(1); //calls logo function with a parameter to show welcome message	
		printf("\n\n\n\n");
		system("Pause");
		
		do
			{
				system("CLS");
				Sleep(750);
				printf("\n\n");
				startingmenu();
				scanf(" %c",&choice);
				
				//goes to different functions based on choice selection
				switch(choice)
					{
						//this case deals with information regarding to customer accounts
						case '1':
							{	
								/*if the user is logged into an account, the function used is the 
								log out function instead of the customer menu function*/ 
								if(strcasecmp(loadeddata.username,"\0")==0)
									customermenu(datastore);
								else
									customerlogout(loadeddata);
								break;
							}// end of case 1
						
						//this case directs users to the search menu function
						case '2':
							{
								search(books);
								break;
							}// end of case 2
						
						
						//this case directs users to the purchasing function
						case '3':
							{
								purchasing(books,loadeddata,purchases);
								break;
							}// end of case 3
						
						//this case directs users to the sales data	
						case '4':
							{
								salesdata();
								break;
							}//end of case 4
						
						//this case allows users to exit the program
						case '5':
							{
								system("pause");
								system("CLS");
								printf("\n\n\t\tAre you sure you want to exit?\n\n\t\tPress '1' to exit\n\t\tPress any other key to cancel\n\n\t\tSelection: ");
								scanf(" %c",&confirmation);  //double checks with the user to ensure that they want to leave
								if(confirmation== '1')
									{
										system("Pause");
										system("CLS");											
										logo(2); //calls logo function with a parameter to show exit message		 	
										system("pause");		
										exit(0);
									}
								//if the user changes their mind, they are redirected back to the main menu
								else if(confirmation != '1')
									{
										printf("\t\tAlright we will redirect you back to the main menu\n");
										choice = 0;
										system("pause");
									}											
								break;
							}// end of case 5
						
						//if the user enters an option not present.
						default:
							{
								printf("\n\t\tThat is not a valid choice. Please ensure you are entering a number that is shown in the menu.\n\n");
								system("Pause");
								system("CLS");
							}// end of default			
					}// end of switch
			}while(choice!=5); // end of most outer do while
	}
