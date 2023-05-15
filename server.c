#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/sem.h>
#include<fcntl.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#define _OPEN_SYS_ITOA_EXT
#define PORT 8080
//This is the semaphore union
union semun{
    int val;
    struct semid_ds *buf;
    unsigned short int *array;
};

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
void acquirep(){
    // printf("Trying to get the lockp\n");
    key_t key = ftok(".",'p');
    int semid = semget(key,1,IPC_CREAT | 0744);
    struct sembuf buf = {0,-1,SEM_UNDO};
    semop(semid,&buf,1);
    // printf("Got the lockp\n");
}
void releasep(){
    // printf("Trying to return the lockp\n");
    key_t key = ftok(".",'p');
    int semid = semget(key,1,IPC_CREAT | 0744);
    struct sembuf buf = {0,1,SEM_UNDO};
    semop(semid,&buf,1);
    // printf("Released the lockp\n");
}
void acquirec(){
    // printf("Trying to get the lockc\n");
    key_t key = ftok(".",'c');
    int semid = semget(key,1,IPC_CREAT | 0744);
    struct sembuf buf = {0,-1,SEM_UNDO};
    semop(semid,&buf,1);
    // printf("Got the lockc\n");
}
void releasec(){
    // printf("Trying to release the lockc\n");
    key_t key = ftok(".",'c');
    int semid = semget(key,1,IPC_CREAT | 0744);
    struct sembuf buf = {0,1,SEM_UNDO};
    semop(semid,&buf,1);
    // printf("Released the lockc\n");
}
void acquireb(){
    // printf("Trying to get the lockb\n");
    key_t key = ftok(".",'b');
    int semid = semget(key,1,IPC_CREAT | 0744);
    struct sembuf buf = {0,-1,SEM_UNDO};
    semop(semid,&buf,1);
    // printf("Got the lockb\n");
}
void releaseb(){
    // printf("Trying to release the lockb\n");
    key_t key = ftok(".",'b');
    int semid = semget(key,1,IPC_CREAT | 0744);
    struct sembuf buf = {0,1,SEM_UNDO};
    semop(semid,&buf,1);
    // printf("Released the lockb\n");
}
//This function adds the products
void addProduct(int productID,char productName[],int quantity,int price,int nsd){
    acquirep();
    int fd = open("Products.dat",O_RDWR | O_CREAT,0744);
    lseek(fd,0,SEEK_END);
    struct Product prod;
    prod.productId = productID;
    strcpy(prod.productName,productName);
    prod.quantity = quantity;
    prod.price = price;
    int bytes = write(fd,&prod,sizeof(prod));
    if(bytes > 0){
        // printf("Product was added\n");
        char string[100] = "Product was added\n";
        write(nsd,string,sizeof(string));
    }
    else{
        // printf("Data was not added\n");
        char string[100] = "Data was not added\n";
        write(nsd,string,sizeof(string));
    }
    close(fd);
    releasep();
}

//This function deletes the products
void delProduct(int productID,int nsd){
    acquirep();
    int fd = open("Products.dat",O_RDWR,0744);
    int fd1 = open("Products1.dat",O_RDWR | O_CREAT,0744);
    struct Product buffer;
    while(read(fd,&buffer,sizeof(buffer))){
        if(buffer.productId != productID){
            write(fd1,&buffer,sizeof(buffer));
        }
    }
    close(fd);
    close(fd1);
    if(remove("Products.dat")==0){
        // printf("Removed the product\n");
        char string[100] = "Removed the product\n";
        write(nsd,string,sizeof(string));
    }
    else{
        // printf("Error in deleting\n");
        char string[100] = "Error in deleting\n";
        write(nsd,string,sizeof(string));
    }
    rename("Products1.dat","Products.dat");
    releasep();
}

//This function updates the products
void update(int prodid,int qty,int price,int nsd){
    acquirep();
    //This function updated the quantity and price of a product
    int fd = open("Products.dat",O_RDWR,0744);
    struct Product buffer;
    while(read(fd,&buffer,sizeof(struct Product)) > 0){
        if(buffer.productId == prodid){
            buffer.quantity = qty;
            buffer.price = price;
            lseek(fd,-sizeof(struct Product),SEEK_CUR);
            write(fd,&buffer,sizeof(buffer));
            // printf("The product has been updated\n");
            char string[100] = "The product has been updated\n";
            write(nsd,string,sizeof(string));
            close(fd);
            releasep();
            return;
        }
    }
    // printf("The product was not updated\n");
    char string[100] = "The product was not updated\n";
    write(nsd,string,sizeof(string));
    releasep();
}

//This function prints all the products
void products(int nsd){
    acquirep();
    //This function should display all the products to the consumer.
    int fd = open("Products.dat",O_RDONLY,0744);
    struct Product buffer;
    int i = 0;
    while(read(fd,&buffer,sizeof(struct Product)) > 0){
        i = i+1;
    }
    lseek(fd,0,SEEK_SET);
    write(nsd,&i,sizeof(i));
    while(read(fd,&buffer,sizeof(struct Product))>0){
        // printf("Product Id: %d\n",buffer.productId);
        // printf("Product Name: %s\n",buffer.productName);
        // printf("Quantity: %d\n",buffer.quantity);
        // printf("Price: %d\n",buffer.price);
        // printf("\n");
        write(nsd,&buffer,sizeof(buffer));
    }
    close(fd);
    releasep();
}

//This function prints all the cart items of a customer
void cart(int CustomerId,int nsd){
    acquirec();
    //This function displays all the cart items to the customer.
    int fd = open("Customer.dat",O_RDONLY,0744);
    struct Customer custom;
    int k = 0;
    while(read(fd,&custom,sizeof(custom))){
        if(CustomerId == custom.CustomerId){
            lseek(fd,-sizeof(struct Customer),SEEK_CUR);
            int fd = open("Products.dat",O_RDWR,0744);
            for(int i=0;i<100;i++){
                if(custom.arr[i].productId!=-1){
                    k = k + 1;
                    // printf("Product Id: %d\n",custom.arr[i].productId);
                    // printf("Product Name: %s\n",custom.arr[i].productName);
                    // printf("Quantity: %d\n",custom.arr[i].quantity);
                    // printf("Price: %d\n",custom.arr[i].price);
                    // printf("\n");
                }
            }
            // printf("The number of products is %d\n",k);
            write(nsd,&k,sizeof(k));
            for(int i=0;i<100;i++){
                if(custom.arr[i].productId!=-1){
                    write(nsd,&custom.arr[i],sizeof(custom.arr[i]));
                }
            }
            close(fd);
            releasec();
            return;
        }
    }
    if(!k){
        write(nsd,&k,sizeof(k));
        close(fd);
    }
    releasec();
}

//The add to cart function adds the product to the corresponding cart
void addtoCart(int Customer_id,int Product_id,int qty,int nsd){
    acquirep();
    acquirec();
    //This function should add the product to the customers cart. 
    int product_fd = open("Products.dat",O_RDONLY,0744);
    int customer_fd = open("Customer.dat",O_RDWR | O_CREAT,0744);
    struct Product prod;
    struct Customer custom;
    while(read(product_fd,&prod,sizeof(struct Product))){
        if(Product_id == prod.productId && qty<= prod.quantity){
            while(read(customer_fd,&custom,sizeof(struct Customer))){
                if(Customer_id == custom.CustomerId){
                    printf("Found customer_id\n");
                    // struct Product newProduct;
                    // newProduct.productId = prod.productId;
                    // strcpy(newProduct.productName,prod.productName);
                    // newProduct.quantity = qty;
                    // newProduct.price =  prod.price;
                    for(int i=0;i<100;i++){
                        if(custom.arr[i].productId == -1){
                            custom.arr[i].productId = prod.productId;
                            strcpy(custom.arr[i].productName,prod.productName);
                            custom.arr[i].quantity = qty;
                            custom.arr[i].price = prod.price;
                            break;
                        }
                    }
                    lseek(customer_fd,-sizeof(custom),SEEK_CUR);
                    write(customer_fd,&custom,sizeof(custom));
                    close(customer_fd);
                    close(product_fd);
                    printf("Added to Cart\n");
                    char string[100] = "Added to cart\n";
                    write(nsd,string,sizeof(string));
                    releasec();
                    releasep();
                    return;
                }
            }
        }
    }
    releasec();
    releasep();
    printf("Could not Add to cart\n");
    char string[100] = "Could not add to cart\n";
    write(nsd,string,sizeof(string));
}

//Edit cart item edits the cart item
void editcartItem(int Customer_id,int Product_id,int qty,int nsd){
    acquirep();
    acquirec();
    //This function should edit the cart item quantity.
    int product_fd = open("Products.dat",O_RDONLY,0744);
    int customer_fd = open("Customer.dat",O_RDWR | O_CREAT,0744);
    struct Product prod;
    struct Customer custom;
    while(read(product_fd,&prod,sizeof(struct Product))){
        if(Product_id == prod.productId && qty<= prod.quantity){
            while(read(customer_fd,&custom,sizeof(struct Customer))){
                if(Customer_id == custom.CustomerId){
                    struct Product newProduct;
                    newProduct.productId = prod.productId;
                    strcpy(newProduct.productName,prod.productName);
                    newProduct.quantity = qty;
                    newProduct.price =  prod.price;
                    for(int i=0;i<100;i++){
                        if(custom.arr[i].productId == Product_id){
                                custom.arr[i] = newProduct;
                                lseek(customer_fd,-sizeof(custom),SEEK_CUR);
                                write(customer_fd,&custom,sizeof(custom));
                                close(customer_fd);
                                close(product_fd);
                                printf("Successfully edited cart item\n");
                                char string[100] = "Successfully edited cart item\n";
                                write(nsd,string,sizeof(string));
                                releasec();
                                releasep();
                                return;
                        }
                    }
                }
            }
        }   
    }
    printf("Could not edit cart item\n");
    char string[100] = "Could not edit cart item\n";
    write(nsd,string,sizeof(string));
    releasec();
    releasep();
    close(customer_fd);
    close(product_fd);
}

//Delete cart item deletes the cart item
void deletecartItem(int Customer_id,int Product_id,int nsd){
    acquirep();
    acquirec();
    int product_fd = open("Products.dat",O_RDONLY,0744);
    int customer_fd = open("Customer.dat",O_RDWR | O_CREAT,0744);
    struct Product prod;
    struct Customer custom;
    while(read(product_fd,&prod,sizeof(struct Product))){
        if(Product_id == prod.productId){
            while(read(customer_fd,&custom,sizeof(struct Customer))){
                if(Customer_id == custom.CustomerId){
                    // struct Product newProduct = {-1,prod.productName,0,prod.price};
                    struct Product newProduct;
                    newProduct.productId = -1;
                    strcpy(newProduct.productName,prod.productName);
                    newProduct.quantity = 0;
                    newProduct.price =  prod.price;
                    for(int i=0;i<100;i++){
                        if(custom.arr[i].productId == Product_id){
                                custom.arr[i] = newProduct;
                                lseek(customer_fd,-sizeof(custom),SEEK_CUR);
                                write(customer_fd,&custom,sizeof(custom));
                                close(customer_fd);
                                close(product_fd);
                                printf("Successfully Deleted Cart item\n");
                                char string[100] = "Successfully Deleted Cart item\n";
                                write(nsd,string,sizeof(string));
                                releasec();
                                releasep();
                                return;
                        }
                    }
                }
            }
        }   
    }
    printf("Could not delete cart item\n");
    char string[100] = "Could not delete cart item\n";
    write(nsd,string,sizeof(string));
    releasec();
    releasep();
}

//This function creates the cart for the customer
void createcart(int customerId,int sd){
    acquirec();
    int fd = open("Customer.dat",O_RDWR | O_CREAT,0744);
    lseek(fd,0,SEEK_END);
    struct Customer custom;
    struct Product initializer = {-1,"None",-1,-1};
    for(int i = 0; i<100;i++){
        custom.arr[i] = initializer;
    }
    custom.CustomerId = customerId;
    write(fd,&custom,sizeof(custom));
    close(fd);
    releasec();
}

void buyCart(int customerId,int nsd){
    acquirec();
    acquirep();
    int fd = open("Customer.dat",O_RDWR,0744);
    struct Customer custom;
    struct Product prod;
    while(read(fd,&custom,sizeof(struct Customer)) > 0){
        //Check if the customer id is valid or not
        if (custom.CustomerId == customerId){
            int amount = 0;
            int flag = 1;
            int number = 0;
            //This is the for loop which checks if the cart can be bought or not
            //This also finds the amount to be paid to buy the cart
            for(int i=0;i<100;i++){
                int find = 0;
                if(custom.arr[i].productId>0){
                    int fd2 = open("Products.dat",O_RDWR,0744);
                    while(read(fd2,&prod,sizeof(struct Product)) > 0){
                        if((prod.productId == custom.arr[i].productId) && (prod.quantity > 0) && (prod.quantity>=custom.arr[i].quantity)){
                            find = 1;
                            amount = amount + custom.arr[i].price*custom.arr[i].quantity;
                            number = number + 1;
                        }
                    }
                    if(!find){
                        flag = 0;
                    }
                    close(fd2);
                }
                if(!flag){
                    break;
                }
            }
            if(!flag || amount == 0){
                printf("Cart can't be bought");
                char string[100] = "Cart can't be bought\n";
                write(nsd,string,sizeof(string));
                releasec();
                releasep();
                return;
            }
            else{
                //This is the confirmation condition
                int amount_pay;
                printf("The amount to be paid is %d\n",amount);
                char string1[100] = "Cart can be bought\n";
                write(nsd,string1,sizeof(string1));
                // printf("Enter amount to confirm: ");
                char string2[100] = "Enter the amount to confirm:\n";
                write(nsd,&amount,sizeof(amount));
                write(nsd,string2,sizeof(string2));
                read(nsd,&amount_pay,sizeof(amount_pay));
                // scanf("%d",&amount_pay);
                char string3[100] = "Please pay the correct amount\n";
                while(amount_pay != amount){
                    // printf("Please pay the correct amount\n");
                    write(nsd,string3,sizeof(string3));
                    read(nsd,&amount_pay,sizeof(amount_pay));
                }
                char string4[100] = "Successfully paid\n";
                write(nsd,string4,sizeof(string4));
                // char filename[100];
                // sprintf(filename, "%d", customerId);
                // char fileext[20] = ".txt";
                // strcat(filename,fileext);
                // int logfd = open(filename,O_CREAT | O_RDWR,0744);
                // char opening[20] = "The Receipt\n";
                // write(logfd,opening,sizeof(opening));
                // char num[10];
                if(amount_pay == amount){
                    //The for loop which buys the product and updates the quantity
                    //in the product text
                    write(nsd,&number,sizeof(number));
                    for(int i=0;i<100;i++){
                        if(custom.arr[i].productId > 0){
                            int fd2 = open("Products.dat",O_RDWR,0744);
                            while (read(fd2,&prod,sizeof(struct Product)) > 0)
                            {
                                if(custom.arr[i].productId == prod.productId){
                                    // write(logfd,"Product ID: ",sizeof("Product ID: "));
                                    // sprintf(num,"%d",custom.arr[i].productId);
                                    // write(logfd,num,sizeof(num));
                                    // write(logfd,"\n",sizeof("\n"));
                                    // write(logfd,"Product Name: ",sizeof("Product Name: "));
                                    // write(logfd,custom.arr[i].productName,sizeof(custom.arr[i].productName));
                                    // write(logfd,"\n",sizeof("\n"));
                                    // write(logfd,"Quantity: ",sizeof("Quantity: "));
                                    // sprintf(num,"%d",custom.arr[i].quantity);
                                    // write(logfd,num,sizeof(num));
                                    // write(logfd,"\n",sizeof("\n"));
                                    // write(logfd,"Price: ",sizeof("Price: "));
                                    // sprintf(num,"%d",custom.arr[i].price);
                                    // write(logfd,num,sizeof(num));
                                    // write(logfd,"\n",sizeof("\n"));
                                    // write(logfd,"\n",sizeof("\n"));
                                    write(nsd,&custom.arr[i],sizeof(custom.arr[i]));
                                    prod.quantity = prod.quantity - custom.arr[i].quantity;
                                    break;
                                }
                            }
                            // write(logfd,"The final amount is ",21);
                            // sprintf(num,"%d",amount);
                            // write(logfd,num,sizeof(num));
                            // write(logfd,"\n",sizeof("\n"));
                            lseek(fd2,-sizeof(prod),SEEK_CUR);
                            write(fd2,&prod,sizeof(prod));
                            close(fd2);
                        }
                    }
                }
            }
        }
    }
    lseek(fd,0,SEEK_SET);
    // int fd1 = open("Customer.dat",O_RDWR,0744);
    int fd2 = open("Customer1.dat",O_RDWR | O_CREAT,0744);
    while(read(fd,&custom,sizeof(struct Customer))>0){
        if(custom.CustomerId != customerId){
            write(fd2,&custom,sizeof(struct Customer));
        }
    }
    close(fd);
    close(fd2);
    if(remove("Customer.dat")==0){
        rename("Customer1.dat","Customer.dat");
    }
    releasec();
    releasep();
    createcart(customerId,nsd);
}
//The following function calls the admin
void Server(){
    int fdc = open("Cid.txt",O_CREAT,0744);
    int cid = 1;
    write(fdc,&cid,sizeof(cid));
    close(fdc);
    union semun arg1,arg2,arg3;
    key_t key1 = ftok(".",'p');
    key_t key2 = ftok(".",'c');
    key_t key3 = ftok(".",'b');
    int semid1 = semget(key1,1,IPC_CREAT | 0744);
    int semid2 = semget(key2,1,IPC_CREAT | 0744);
    int semid3 = semget(key3,1,IPC_CREAT | 0744);
    arg1.val = 1;
    arg2.val = 1;
    arg3.val = 1;
    semctl(semid1,0,SETVAL,arg1);
    semctl(semid2,0,SETVAL,arg2);
    semctl(semid3,0,SETVAL,arg3);
    struct sockaddr_in serv,cli;
    int sd;
    sd = socket(AF_INET,SOCK_STREAM,0);
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(PORT);
    bind(sd, (struct sockaddr *)&serv, sizeof(serv));
    listen(sd,5);
    int nsd;
    int choicenum;
    int len = sizeof(cli);
    // printf("%d\n",len);
    int value;
    char buf[100];
    while(1){
        nsd = accept(sd, (struct sockaddr *)&cli, &len);
        if(!fork()){
            int usertype;
            read(nsd,&usertype,sizeof(usertype));
            if(usertype==1){
                while(1){
                    read(nsd,&choicenum,sizeof(choicenum));
                    if(choicenum == 1){
                        int product_id,quantity,price;
                        char product_name[100];
                        read(nsd,&product_id,sizeof(product_id));
                        read(nsd,product_name,sizeof(product_name));
                        read(nsd,&quantity,sizeof(quantity));
                        read(nsd,&price,sizeof(price));
                        addProduct(product_id,product_name,quantity,price,nsd);
                    }
                    else if (choicenum == 2){
                        int product_id;
                        read(nsd,&product_id,sizeof(product_id));
                        delProduct(product_id,nsd);
                    }
                    else if (choicenum == 3){
                        int product_id,quantity,price;
                        read(nsd,&product_id,sizeof(product_id));
                        read(nsd,&quantity,sizeof(quantity));
                        read(nsd,&price,sizeof(price));
                        update(product_id,quantity,price,nsd);
                    }
                    else if (choicenum == 4){
                        products(nsd);
                    }
                    else{
                        products(nsd);
                        close(nsd);
                        exit(0);
                    }
                }
            }
            else if (usertype==2){
                int option;
                read(nsd,&option,sizeof(option));
                if(option == 2){
                    fdc = open("Cid.txt",O_RDWR,0744);
                    read(fdc,&cid,sizeof(cid));
                    write(nsd,&cid,sizeof(cid));
                    createcart(cid,nsd);
                    cid = cid + 1;
                    lseek(fdc,0,SEEK_SET);
                    write(fdc,&cid,sizeof(cid));
                    close(fdc);
                }
                while(1){
                    read(nsd,&choicenum,sizeof(choicenum));
                    if(choicenum == 1){
                        //This choice is for adding the product to cart
                        //For this we need to know customer Id, product id and quantity
                        int customer_id,product_id,qty;
                        read(nsd,&customer_id,sizeof(customer_id));
                        read(nsd,&product_id,sizeof(product_id));
                        read(nsd,&qty,sizeof(qty));
                        // printf("Done till AddtoCart\n");
                        addtoCart(customer_id,product_id,qty,nsd);
                    }
                    else if(choicenum == 2){
                        int customer_id,product_id;
                        read(nsd,&customer_id,sizeof(customer_id));
                        read(nsd,&product_id,sizeof(customer_id));
                        deletecartItem(customer_id,product_id,nsd);
                    }
                    else if(choicenum == 3){
                        int customer_id,product_id,qty;
                        read(nsd,&customer_id,sizeof(customer_id));
                        read(nsd,&product_id,sizeof(product_id));
                        read(nsd,&qty,sizeof(qty));
                        editcartItem(customer_id,product_id,qty,nsd);
                    }
                    else if(choicenum == 4){
                        products(nsd);
                    }
                    else if(choicenum == 5){
                        int customer_id;
                        read(nsd,&customer_id,sizeof(customer_id));
                        cart(customer_id,nsd);
                    }
                    else if(choicenum == 6){
                        int customer_id;
                        read(nsd,&customer_id,sizeof(customer_id));
                        buyCart(customer_id,nsd);
                    }
                    else if(choicenum == 7){
                        close(nsd);
                        exit(0);
                    }
                }
            }
        }
        else{
            close(nsd);
        }
    }
}



int main(){
    printf("This is the server side program\n");
    Server();
}