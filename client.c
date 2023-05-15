#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/sem.h>
#include<fcntl.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#define PORT 8080

//This is the product structure
struct Product{
    int productId;
    char productName[100];
    int quantity;
    int price;
};

//This is the customer structure
struct Customer{
    int CustomerId;
    struct Product arr[100];
};

//The following function prints the login Menu
void loginMenu(){
    printf("Enter one of the two options to Login\n");
    printf("1 - Admin\n");
    printf("2 - User\n");
}

//The following function prints the admin menu
void Adminmenu(){
    printf("Enter one of the following options\n");
    printf("1 - Add a product\n");
    printf("2 - Delete a product\n");
    printf("3 - Update Quantity of product\n");
    printf("4 - List of products\n");
    printf("5 - Logout\n");
}

//The following function prints the user menu
void Usermenu(){
    printf("Enter any of the following options\n");
    printf("1 - Add Product to Cart\n");
    printf("2 - Delete Product from Cart\n");
    printf("3 - Edit cart item\n");
    printf("4 - List all Products\n");
    printf("5 - List cart items\n");
    printf("6 - Proceed to Pay\n");
    printf("7 - Logout\n");
}

//The following function calls the user
void User(){
    struct sockaddr_in serv;
    int sd;
    sd = socket(AF_INET, SOCK_STREAM,0);
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv.sin_port = htons(PORT);
    connect(sd, (struct sockaddr *)&serv, sizeof(serv));
    int usertype = 2;
    int value;
    write(sd,&usertype,sizeof(usertype));
    printf("Do you have a customer id?: \n");
    printf("Enter 1 if 'Yes' and 2 if 'No': ");
    int option;
    scanf("%d",&option);
    write(sd,&option,sizeof(option));
    int customer_id;
    if(option == 2){
        read(sd,&customer_id,sizeof(customer_id));
        printf("You're customer id is %d\n",customer_id);
        printf("Please use this id for further purposes\n");
    }
    struct Product prod;
    while(1){
        Usermenu();
        scanf("%d",&value);
        write(sd,&value,sizeof(value));
        if(value==1){
            int customer_id,product_id,qty;
            char string[100];
            printf("Enter the customer id: ");
            scanf("%d",&customer_id);
            printf("Enter the product id of the product to be added: ");
            scanf("%d",&product_id);
            printf("Enter the quantity to be added: ");
            scanf("%d",&qty);
            write(sd,&customer_id,sizeof(customer_id));
            write(sd,&product_id,sizeof(product_id));
            write(sd,&qty,sizeof(qty));
            read(sd,string,sizeof(string));
            printf("%s",string);
        }
        else if(value==2){
            char string[100];
            int customer_id,product_id;
            printf("Enter the customer id: ");
            scanf("%d",&customer_id);
            printf("Enter the product id of the product to be deleted: ");
            scanf("%d",&product_id);
            write(sd,&customer_id,sizeof(customer_id));
            write(sd,&product_id,sizeof(product_id));
            read(sd,string,sizeof(string));
            printf("%s",string);
        }
        else if(value == 3){
            char string[100];
            int customer_id,product_id,qty;
            printf("Enter the customer id: ");
            scanf("%d",&customer_id);
            printf("Enter the product id of the product to be edited: ");
            scanf("%d",&product_id);
            printf("Enter the new quantity: ");
            scanf("%d",&qty);
            write(sd,&customer_id,sizeof(customer_id));
            write(sd,&product_id,sizeof(product_id));
            write(sd,&qty,sizeof(qty));
            read(sd,string,sizeof(string));
            printf("%s",string);    

        }
        else if(value == 4){
            int no;
            read(sd,&no,sizeof(no));
            struct Product prod;
            for(int i=0;i<no;i++){
                read(sd,&prod,sizeof(struct Product));
                printf("Product Id: %d\n",prod.productId);
                printf("Product Name: %s\n",prod.productName);
                printf("Quantity left: %d\n",prod.quantity);
                printf("Price of the product: %d\n",prod.price);
                printf("\n");
            }
        }
        else if(value == 5){
            int customer_id;
            printf("Enter the customer id: ");
            scanf("%d",&customer_id);
            write(sd,&customer_id,sizeof(customer_id));
            int no;
            read(sd,&no,sizeof(no));
            struct Product prod;
            printf("The number of products is %d\n",no);
            for(int i=0; i<no ; i++){
                read(sd,&prod,sizeof(struct Product));
                printf("Product Id: %d\n",prod.productId);
                printf("Product Name: %s\n",prod.productName);
                printf("Quantity: %d\n",prod.quantity);
                printf("Price of the product: %d\n",prod.price);
                printf("\n");
            }
        }
        else if(value == 6){
            int customer_id;
            printf("Enter the customer id: ");
            scanf("%d",&customer_id);
            write(sd,&customer_id,sizeof(customer_id));
            char string[100] = "Cart can be bought\n";
            char string1[100];
            read(sd,string1,sizeof(string1));
            printf("%s",string1);
            int amount;
            int number;
            if(strcmp(string,string1)==0){
                read(sd,&amount,sizeof(amount));
                printf("Amount to be paid is %d\n",amount);
                read(sd,string1,sizeof(string1));
                char string2[100] = "Successfully paid\n";
                while(strcmp(string1,string2)!=0){
                    printf("Enter the amount: ");
                    scanf("%d",&amount);
                    write(sd,&amount,sizeof(amount));
                    read(sd,string1,sizeof(string1));
                    printf("%s",string1);
                }
                read(sd,&number,sizeof(number));
                char filename[100];
                sprintf(filename, "%d", customer_id);
                char fileext[20] = ".txt";
                strcat(filename,fileext);
                int logfd = open(filename,O_CREAT | O_RDWR,0744);
                char opening[20] = "The Receipt\n";
                write(logfd,opening,sizeof(opening));
                char num[100];
                int size;
                for(int i = 0; i<number;i++){   
                    read(sd,&prod,sizeof(struct Product));
                write(logfd,"Product ID: ",sizeof("Product ID: "));
                size = sprintf(num,"%d",prod.productId);
                // strcat(num,"\n");
                write(logfd,num,size);
                write(logfd,"\n",sizeof("\n"));
                write(logfd,"Product Name: ",sizeof("Product Name: "));
                size = sprintf(num,"%s",prod.productName);
                write(logfd,num,size);
                write(logfd,"\n",sizeof("\n"));
                write(logfd,"Quantity: ",sizeof("Quantity: "));
                size = sprintf(num,"%d",prod.quantity);
                // strcat(num,"\n");
                write(logfd,num,size);
                write(logfd,"\n",sizeof("\n"));
                write(logfd,"Price: ",sizeof("Price: "));
                size = sprintf(num,"%d",prod.price);
                // strcat(num,"\n");
                write(logfd,num,size);
                write(logfd,"\n",sizeof("\n"));
                write(logfd,"\n",sizeof("\n"));
                // printf("Product Id: %d\n",prod.productId);
                // printf("Product Name: %s\n",prod.productName);
                // printf("Quantity left: %d\n",prod.quantity);
                // printf("Price of the product: %d\n",prod.price);
                // printf("\n");
                }
                write(logfd,"The final amount is ",21);
                size = sprintf(num,"%d",amount);
                write(logfd,num,size);
                write(logfd,"\n",sizeof("\n"));
                close(logfd);
            }
        }
        else{
            write(sd,"End",sizeof("End"));
            close(sd);
            exit(0);
        }
    }
}

void Admin(){
    struct sockaddr_in serv;
    int sd;
    sd = socket(AF_INET, SOCK_STREAM,0);
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv.sin_port = htons(PORT);
    connect(sd, (struct sockaddr *)&serv, sizeof(serv));
    int usertype = 1;
    int value;
    write(sd,&usertype,sizeof(usertype));
    while(1){
        Adminmenu();
        scanf("%d",&value);
        write(sd,&value,sizeof(value));
        if(value == 1){
            int product_id,quantity,price;
            char product_name[100];
            printf("Enter the product Id: ");
            scanf("%d",&product_id);
            printf("Enter the product name: ");
            scanf("%s",product_name);
            printf("Enter the quantity: ");
            scanf("%d",&quantity);
            printf("Enter the price of the product: ");
            scanf("%d",&price);
            write(sd,&product_id,sizeof(product_id));
            write(sd,product_name,sizeof(product_name));
            write(sd,&quantity,sizeof(quantity));
            write(sd,&price,sizeof(price));
            char string[100];
            read(sd,string,sizeof(string));
            printf("%s",string);
        }
        else if(value == 2){
            int product_id;
            printf("Enter the product id: ");
            scanf("%d",&product_id);
            write(sd,&product_id,sizeof(product_id));
            char string[100];
            read(sd,string,sizeof(string));
            printf("%s",string);
        }
        else if(value == 3){
            int product_id,quantity,price;
            printf("Enter the product id: ");
            scanf("%d",&product_id);
            printf("Enter the quantity: ");
            scanf("%d",&quantity);
            printf("Enter the price: ");
            scanf("%d",&price);
            write(sd,&product_id,sizeof(product_id));
            write(sd,&quantity,sizeof(quantity));
            write(sd,&price,sizeof(price));
            char string[100];   
            read(sd,string,sizeof(string));
            printf("%s",string);
        }
        else if(value == 4){
            int no;
            read(sd,&no,sizeof(no));
            struct Product prod;
            for(int i=0;i<no;i++){
                read(sd,&prod,sizeof(struct Product));
                printf("Product Id: %d\n",prod.productId);
                printf("Product Name: %s\n",prod.productName);
                printf("Quantity left: %d\n",prod.quantity);
                printf("Price of the product: %d\n",prod.price);
                printf("\n");
            }
        }
        else{
            // printf("Value: %d\n",value);
            int number;
            read(sd,&number,sizeof(number));
            struct Product prod;
            int logfd = open("Logfile.txt",O_CREAT | O_RDWR,0744);
            int size;
            char num[100];
            for(int i=0;i<number;i++){
                read(sd,&prod,sizeof(struct Product));
                write(logfd,"Product ID: ",sizeof("Product ID: "));
                size = sprintf(num,"%d",prod.productId);
                // strcat(num,"\n");
                write(logfd,num,size);
                write(logfd,"\n",sizeof("\n"));
                write(logfd,"Product Name: ",sizeof("Product Name: "));
                size = sprintf(num,"%s",prod.productName);
                write(logfd,num,size);
                write(logfd,"\n",sizeof("\n"));
                write(logfd,"Quantity: ",sizeof("Quantity: "));
                size = sprintf(num,"%d",prod.quantity);
                // strcat(num,"\n");
                write(logfd,num,size);
                write(logfd,"\n",sizeof("\n"));
                write(logfd,"Price: ",sizeof("Price: "));
                size = sprintf(num,"%d",prod.price);
                // strcat(num,"\n");
                write(logfd,num,size);
                write(logfd,"\n",sizeof("\n"));
                write(logfd,"\n",sizeof("\n"));
                // printf("Product Id: %d\n",prod.productId);
                // printf("Product Name: %s\n",prod.productName);
                // printf("Quantity left: %d\n",prod.quantity);
                // printf("Price of the product: %d\n",prod.price);
                // printf("\n");
            }
            close(logfd);
            close(sd);
            exit(0);
        }
    }
}

int main(){
    //This is the client side program
    int option;
    loginMenu();
    scanf("%d",&option);
    if(option == 1){
        // printf("Yes admin\n");
        Admin();
    }
    else if(option == 2){
        User();
    }
}