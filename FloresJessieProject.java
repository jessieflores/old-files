import javax.swing.JOptionPane;
import java.util.Scanner;
import java.util.InputMismatchException;
import java.io.*;


/**
 * Menu driven program, takes two files in commandline arguments, 
 * creates a telephone book of people from the first file and writes 
 * in the second. Menu used to create, delete, get listings
 * for people in the book
 * 
 * @author Jessie Flores
 * @assignment ICS 211 Final Project
 * @date December 14 2015
 */
 

public class FloresJessieProject {
   
   public static void main(String[] args) {
   
      TelephoneBook book = new TelephoneBook();   
      //if there isn't exactly two files
      if(args.length != 2) {
         System.out.println("Error: Please enter two file names as arguments.");
      }
      //otherwise, continue the program
      else {
         readFromFile(args[0],book);
         displayMenu(book);
         writeToFile(args[1],book);
      }
      
   }
   
  /**
   * Reads file given in 1st commandline argument and adds elements to 
   * telephone book object
   * 
   * @param fileName input file for reading to book
   * @param book telephone book object for people read on fileName
   */   
   public static void readFromFile(String fileName, TelephoneBook book) {
   
      String name, line, phoneNumber;
      Scanner input = null;
      //check if file exists      
      try {
         input = new Scanner(new File(fileName));
      }
      //if file not found, tell user   
      catch(FileNotFoundException e) {
         System.out.print("\nError: The file \"");
         System.out.println(fileName +"\" cannot be found");
      }
      //if file found, read from it
      if(input != null) {
         System.out.println("Reading from input file: " + fileName);
         //skip 1st line
         input.nextLine();
         //while there is still a line in the file   
         while (input.hasNextLine()) {
            line = input.nextLine();
            //skips all commas in line 
            Scanner lineInput = new Scanner(line).useDelimiter(",");
            name = lineInput.next();
            phoneNumber = lineInput.next();
            //add new listing to telephone book
            book.insert(name,phoneNumber);
         }
      }
      
   }
  /**
   * Creates a menu using system.out that enables user to add, retrieve, 
   * get listings and display the telephone book
   * 
   * @param book telephone book object used for editing and viewing
   * in this method
   */   
   public static void displayMenu(TelephoneBook book) {
   
      int choice = 0;
      String name = "", number = "";
      
      System.out.println("\nPersonal Telephone Book Hash Table");
      //while user has not quitted program
      while(choice != 5) {
         //Display menu
         System.out.println("\nMAIN MENU");
         System.out.println("1. Insert telephone number");
         System.out.println("2. Retrieve telephone number");
         System.out.println("3. Delete telephone number");
         System.out.println("4. Display telephone book");
         System.out.println("5. End program");
         System.out.print("Enter choice (1-5): ");
         
         Scanner keyboardInput = new Scanner(System.in);
         //check if choice is an integer
         try {
            choice = keyboardInput.nextInt();
         } 
         //if choice isn't an integer display error
         catch(InputMismatchException e) {
            System.out.println("\nError: enter an integer between 1-5");
            choice = 6;
         }
         //if choice is 1, add to phonebook
         if(choice == 1) {
            System.out.print("\nEnter person's name: ");
            name = keyboardInput.next();
            System.out.print("Enter person's telephone number: ");
            number = keyboardInput.next();
            
            book.insert(name,number);
            
         }
         //if choice is 2, get telephone listing
         else if(choice == 2) {
            System.out.print("\nEnter person's name: ");
            name = keyboardInput.next();
            
            System.out.println("The telephone number is: " + book.retrieve(name).getPhoneNumber());
         }
         //if choice is 3, delete the listing
         else if(choice == 3) {
            System.out.print("\nEnter person's name: ");
            name = keyboardInput.next(); 
            
            System.out.println("Deleting: " + book.retrieve(name).toString());
            
            book.delete(name);
         }
         //if choice is 4, display entire telephone book
         else if(choice == 4) {
            System.out.println("\n" + book.toString());
         }
         //if choice is 5, exits while loop
         else if(choice == 5) {
         }
         //if choice is something other than 1-5, display error
         else if(choice != 6 && (choice < 1 || choice > 5)){
            System.out.println("\nError: enter an integer between 1-5");
         }
         
      }
     
   }

   
 /**
   * Writes contents of telephone book to output file
   * 
   * @param outputFile file for writing from telephone book to file
   * @param book telephone book that contains listings to write to file name
   */   
   public static void writeToFile(String outputFile, TelephoneBook book) {
   
      PrintWriter fileWriter = null;
   
      //check if file exists
      try {
         fileWriter = new PrintWriter(outputFile);
      }
      //inform user if file not found 
      catch (FileNotFoundException exception) {
      
         System.out.print("\nERROR: File not found for \"");
         System.out.println(outputFile + "\"");
      }
   
   
      //if file exists, continue write to file
      if(fileWriter != null) {
         fileWriter.println("name,telephone"); 
         
         //while there are still "pages" in the telephone book
         for(int i = 0; i < 7; i++) 
            fileWriter.print(book.getList(i).writeList());
           
         //inform user and close filewriter
         System.out.println("\nWrote to file \"" + outputFile + "\"");
         fileWriter.close();
      
      }
     
   }     

}    

/**
 * Class PersonNode stores and displays the name and phone number for a person or 
 * multiple persons by using a linked list
 * 
 * @author Jessie Flores
 */
class PersonNode {
   //data fields are name, phone number, and next
   private String name, phoneNumber;
   private PersonNode next;
      
/**
 * Constructor for class PersonNode   
 *
 * @param name stores name of person into data field
 * @param number stores phone number of person into data field
 * @param next stores address of next person node object in list
 */      
   public PersonNode(String name, String number, PersonNode next) {
      this.name = name;
      this.phoneNumber = number;
      this.next = next;
   }
         
/**
 * toString method for class PersonNode   
 *
 * @return a string with info of the person object
 */   
   public String toString() {
      String info = name + " " + phoneNumber; 
      return info;
   }   
      
/**
 * Accessor method for class PersonNode
 *
 * @return a PersonNode object that is next in list
 */
   public PersonNode getNext() {
      return next;
   }   
      
/**
 * Mutator method for class PersonNode
 *
 * @param next sets pointer to next PersonNode object
 */
   public void setNext(PersonNode next) {
      this.next = next;
   } 

/**
 * Accessor method for class PersonNode
 *
 * @return person's phone number
 */
   public String getPhoneNumber() {
      return phoneNumber;
   }
   
   
/**
 * Accessor method for class PersonNode
 *
 * @return name of person
 */
   public String getName() {
      return name;
   }      
     
}

/**
 * Class LinkedList creates a list of PersonNode objects. It can add
 * to the list or output string of the list
 * 
 * @author Jessie Flores
 */
class LinkedList {
   //data field (private PersonNode head)
   private PersonNode head = null;
      
/**
 * adds given parameters to object PersonNode and adds the object to the 
 * front of the list   
 *
 * @param name person's name to be put in object in list
 * @param number person's phone number to be put in object in list
 */   
   public void add(String name, String number) {
      
      PersonNode previous = null;
      //if list is empty, intialize 1st person node object
      if(head == null) 
         head = new PersonNode(name,number,null);
      //otherwise, add new person to front of list  
      else {
         previous = head;
         head = new PersonNode(name,number,null);
         head.setNext(previous);
      }   
   
   }
   
   /**
	 * Gets person node in linked list with the same name as given param.
	 * 
    * @param name string used for checking person with same name
    * @return person node with same name as given param name
	 */
   public PersonNode get(String name) {
   
      PersonNode current = head;  
      //while name doesn't match or reached end of list
      while(!name.equals(current.getName()) || current == null) 
         current = current.getNext();
   
      return current;
      
   }
   
	/**
	 * Removes person node from list.
	 * 
	 * @param name used to find person node to remove
	 */
   public void remove(String name) {
      //if person to be removed is in front of list, set head to head's getNext
      if(name.equals(head.getName()))
         head = head.getNext();
      //otherwise find the person node with the same name and person after that
      else {
      
         PersonNode previous = head;
         PersonNode current = head.getNext();
      	//while name doesn't match
         while(!name.equals(current.getName())) {
         	//set previous to current and current to next
            previous = current;
            current = current.getNext();
         }
      	//set previous node to current's next thus removing current
         previous.setNext(current.getNext());
      }
   
   }
	/**
	 * Writes a list to be used for writing to file.
	 * 
	 * @return String of listings of each person node in list
	 */   
   public String writeList() {
   
      String list = "";
      //if list empty, return blank
      if(head == null)
         return "";
      //if list has only one element, return head's info   
      else if(head.getNext() == null)
         return head.getName() + "," + head.getPhoneNumber() + "\n";
      //otherwise, make list of people in linked list   
      else {      
         list = head.getName() + "," + head.getPhoneNumber() + "\n";
         //while we haven't reached end of list
         for(PersonNode current = head.getNext(); current != null;
            current = current.getNext())
            //keep adding to list   
            list += current.getName() + "," + current.getPhoneNumber() + "\n";
         
         return list;
      }
   }      
                   
      
/**
 * toString method for class LinkedList   
 *
 * @return string that displays part of hash table for separate chaining
 */
   public String toString() {
      //if list empty, return null string
      if(head == null)
         return "null";
      //if list has one person, return head info   
      else if(head.getNext() == null)
         return head.toString() + " => null";
      //otherwise, make table of separate chaining      
      else {   
         //add 1st person's toString
         String info = head.toString() + " => ";
         //keep adding object's toString until end of list  
         for(PersonNode current = head.getNext();
         current != null; current = current.getNext()) { 
            info += current.toString() + " => ";
         }
         info += "null";
         return info;
      }
   }
                    

} 

/**
 * Class TelephoneBook creates a telephone book that uses hashing
 * 
 * @author Jessie Flores
 */
class TelephoneBook {

   private LinkedList[] hashArray = new LinkedList[7];
   
   /**
	 * Constructor for class TelephoneBook. initalizes each linked list
    * object in hash array
	 * 
	 */
   public TelephoneBook() {
      //initialize each list object in array
      for(int i = 0; i < hashArray.length; i++) 
         hashArray[i] = new LinkedList();
      
   }
   

   /**
	 * Inserts name and number to array by hashing name.
	 * 
	 * @param name used for hashing and adding to telephone book object
    * @param number number of person to be added to telephone book object
	 */   
   public void insert(String name, String number) {
      //use hash function to determine which array element to use
      int myHash = name.hashCode() % 7;
      //for negative hash values
      myHash = Math.abs(myHash);
      //add to array element 
      hashArray[myHash].add(name, number);
      
   }
   /**
	 * Retrieves person node of same name by using hashing.
	 * 
	 * @param name used for hashing 
	 */
   public PersonNode retrieve(String name) {
      //use hash function to determine which array element to use
      int myHash = name.hashCode() % 7;
      //for negative hash values
      myHash = Math.abs(myHash);
      //get person node with same name
      return hashArray[myHash].get(name);
      
   }
   /**
	 * Deletes person node of same name by using hashing.
	 * 
	 * @param name used for hashing 
	 */   
   public void delete(String name) {
      //use hash function to determine which array element to use
      int myHash = name.hashCode() % 7;
      //for negative hash values
      myHash = Math.abs(myHash);
      //remove person node with same name
      hashArray[myHash].remove(name);
   }
   /**
	 * Accessor method for class TelephoneBook.
	 * 
	 * @param listNum number to determing which array element to return
    * @return Linked list or a "page" of telephonebook 
	 */   
   public LinkedList getList(int listNum) {
   
      return this.hashArray[listNum];
   }   
   /**
	 * toString method for class TelephoneBook.
	 * 
    * @return String of hash table with separate chaining
	 */    
   public String toString() {
         
      String table = "DISPLAY TABLE\n";
      //loop through contents of hash array
      for(int i = 0; i < hashArray.length; i++)
         table += "table[" + i + "] => " + hashArray[i].toString() + "\n";
      
      return table;   
   }
   
}                          
   
