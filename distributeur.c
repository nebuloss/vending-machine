#include "distributeur.h" //le fichier distributeur.h doit impérativement être dans le même répertoire que distributeur.c. Il contient les librairies à inclure, des macros et le prototype de toutes les fonctions.

static product_class* product_list[3];
static float total;

int main(){
    system("stty raw -echo"); //désactive l'affichage des saisies utilisateur et la touche entrer après les saisies.

    SET_WINDOW_DIMENSION(WINDOW_DIMX,WINDOW_DIMY); //remplace par printf("\e[8;%u;%ut",y,x); on dimensionne la fenêtre
    HIDE_CURSOR(); //cache le curseur;


    product_list[0]=create_new_product_class("FRUITS"); //création de différentes classes de produits
    product_list[1]=create_new_product_class("LEGUMES");
    product_list[2]=create_new_product_class("VIANDES");

    fill_surface(1,1,52,39,LEFT_SIDE_COLOR); //left side x1=1 y1=1 x2=52 y2=40 couleur=5
    for (int i=0;i<3;i++){
        display_product_class(product_list[i],2,10*i+2,7,2);
    }
    fill_surface(2,32,51,37,BASKET_BORDER); //draw basket
    fill_surface(3,32,50,36,LEFT_SIDE_COLOR);

    fill_surface(53,1,90,39,RIGHT_SIDE_COLOR); //right side

    char character[15]="123456789F0L\eV\r"; //initialization of the keys
    key vending_machine_key[15];

    for (int i=0,k=0;i<5;i++){ //initialize and display keys
        for (int j=0;j<3;j++){
            vending_machine_key[k]=(key){character[k],62+7*j,19+4*i};
            display_key(vending_machine_key[k],KEYS_BACKGROUND,KEYS_FOREGROUND);
            k++;
        }
    }

    FILL_SCREEN(SCREEN_BACKGROUND); //screen

    PRINTF_SCREEN(13,4,SCREEN_BACKGROUND,SCREEN_INFORMATION_COLOR,"Bienvenue!");
    PRINTF_SCREEN(0,5,SCREEN_BACKGROUND,SCREEN_WAIT_COLOR,"Pressez [entrer] pour commencer...");

    while(get_vending_machine_key(vending_machine_key,15)!='\r'); //pause

    linked_list* basket=linked_list_add_element(NULL,NULL); //initialisation du panier
    vending_machine_function current_function={main_menu}; //initialisation de la première fonction.

    while (current_function.func!=NULL){ //boucle principale
        current_function=current_function.func(basket,vending_machine_key,15); //lance la première fonction qui retourne la fonction suivante à lancer.
    }

    free_linked_list(basket,false); //libération de la mémoire
    for (int i=0;i<3;i++){
        free_linked_list(product_list[i]->article_list,true);
        free(product_list[i]);
    }

    SHOW_CURSOR();
    CLEAR_SCREEN();
    system("stty echo cooked");
    putchar('\n'); //retour à la ligne
}

vending_machine_function main_menu(linked_list* basket,key* vending_machine_keys,int key_number){
    total=0;
    FILL_SCREEN(SCREEN_BACKGROUND);
    PRINTF_SCREEN(0,0,SCREEN_BACKGROUND,SCREEN_TITLE_COLOR,"Veuillez faire votre choix:");
    PRINTF_SCREEN(0,1,SCREEN_BACKGROUND,SCREEN_MENU_COLOR,"[Touche 1] Ajouter un article");
    PRINTF_SCREEN(0,2,SCREEN_BACKGROUND,SCREEN_MENU_COLOR,"[Touche 2] Supprimer un article");
    PRINTF_SCREEN(0,3,SCREEN_BACKGROUND,SCREEN_MENU_COLOR,"[Touche 3] Payer la commande");
    PRINTF_SCREEN(0,4,SCREEN_BACKGROUND,SCREEN_MENU_COLOR,"[Touche 4] Quitter");
    if(basket->next!=NULL){
        int i=0;
        article* a;
        for(basket=basket->next;basket!=NULL;basket=basket->next){
            a= basket->element;
            total+=a->price;
            PRINTF_SCREEN(0,7+i,SCREEN_BACKGROUND,SCREEN_BASKET_COLOR,"-%.15s (%.1f€)", a->name,a->price);
            i++;
        }
    }else{
        PRINTF_SCREEN(0,7,SCREEN_BACKGROUND,SCREEN_HIGHLIGHT_COLOR,"(vide)");
    }
    PRINTF_SCREEN(0,6,SCREEN_BACKGROUND,SCREEN_TITLE_COLOR,"Votre panier (total:%.1f€):",total);

    int c=get_vending_machine_key(vending_machine_keys,key_number);
    switch(c){
        case '1': return (vending_machine_function){add_element};break;
        case '2': return (vending_machine_function){remove_article};break;
        case '3': return (vending_machine_function){pay};break;
        case '4': return (vending_machine_function){NULL};break; //quitter
        default : return (vending_machine_function){main_menu};
    }
}

vending_machine_function remove_article(linked_list* basket,key* vending_machine_keys,int key_number){
    article* a;
    int i=1,c;
    FILL_SCREEN(SCREEN_BACKGROUND);
    if (basket->next!=NULL){
        PRINTF_SCREEN(0,0,SCREEN_BACKGROUND,SCREEN_TITLE_COLOR,"Veuillez faire votre choix:");
        PRINTF_SCREEN(0,1,SCREEN_BACKGROUND,SCREEN_HIGHLIGHT_COLOR,"[Touche 0] Vider le panier");
        for (linked_list* l=basket->next;l!=NULL;l=l->next){
            a=l->element;
            PRINTF_SCREEN(0,2+i,SCREEN_BACKGROUND,SCREEN_MENU_COLOR,"[Touche %d] Retirer \"%.15s\"",i,a->name);
            i++;
        }
        PRINTF_SCREEN(0,3+i,SCREEN_BACKGROUND,SCREEN_HIGHLIGHT_COLOR,"[Echap] Annuler");

        c=get_vending_machine_key(vending_machine_keys,key_number);
        FILL_SCREEN(SCREEN_BACKGROUND);
        if (c>='1' && c<='9'){
            if(linked_list_remove_range(basket,c-'0')==NULL){
                return (vending_machine_function){remove_article};
            }
        }else if (c=='0'){
            free_linked_list(basket->next,false);
            basket->next=NULL;
        }else if (c!='\e'){
            return (vending_machine_function){remove_article};
        }
    }else{
        PRINTF_SCREEN(0,0,SCREEN_BACKGROUND,SCREEN_ERROR_COLOR,"Aucun article dans le panier!");
        screen_wait_message(3,1);
    }
    return (vending_machine_function){main_menu};
}

vending_machine_function pay(linked_list* basket,key* vending_machine_keys,int key_number){
    FILL_SCREEN(SCREEN_BACKGROUND);
    PRINTF_SCREEN(0,0,SCREEN_BACKGROUND,SCREEN_TITLE_COLOR,"Régler la commande:");

    if (total>0){
        char c;//argent inséré dans le distributeur
        float values[7]={0.1,0.2,0.5,1,2,5,10},input_price; //valurs acceptés en euros
        PRINTF_SCREEN(0,1,SCREEN_BACKGROUND,SCREEN_INFORMATION_COLOR,"Prix total des articles:%.1f€",total);
        PRINTF_SCREEN(0,4,SCREEN_BACKGROUND,SCREEN_TITLE_COLOR,"Veuillez insérer de l'argent:");

        for (int i=0;i<3;i++) PRINTF_SCREEN(0,5+i,SCREEN_BACKGROUND,SCREEN_MENU_COLOR,"[Touche %d] ajouter %.f centimes",i,100*values[i]);
        for (int i=3;i<7;i++) PRINTF_SCREEN(0,5+i,SCREEN_BACKGROUND,SCREEN_MENU_COLOR,"[Touche %d] ajouter %.f euro(s)",i,values[i]);

        PRINTF_SCREEN(0,13,SCREEN_BACKGROUND,SCREEN_HIGHLIGHT_COLOR,"Pressez [Echap] pour annuler");

        while (input_price<total){
            PRINTF_SCREEN(0,2,SCREEN_BACKGROUND,SCREEN_INFORMATION_COLOR,"Reste à payer:%.1f€ \n",total-input_price);
            c=get_vending_machine_key(vending_machine_keys,key_number);
            if (c>='0' && c<='6') input_price+=values[c-'0'];
            else if (c=='\e'){
                FILL_SCREEN(SCREEN_BACKGROUND);
                PRINTF_SCREEN(0,0,SCREEN_BACKGROUND,SCREEN_INFORMATION_COLOR,"Rendu:%.1f€",input_price);
                screen_wait_message(5,1);
                return (vending_machine_function){main_menu};
            }
        }

        FILL_SCREEN(SCREEN_BACKGROUND);
        PRINTF_SCREEN(0,0,SCREEN_BACKGROUND,SCREEN_TITLE_COLOR,"Votre commande a bien été validée!");
        PRINTF_SCREEN(0,1,SCREEN_BACKGROUND,SCREEN_INFORMATION_COLOR,"Rendu:%.1f€",input_price-total);
        return (vending_machine_function){end_part};
    }else{
        PRINTF_SCREEN(0,2,SCREEN_BACKGROUND,SCREEN_ERROR_COLOR,"Aucun article à payer.");
        screen_wait_message(3,6);
        return (vending_machine_function){main_menu};
    }
}

vending_machine_function end_part(linked_list* basket,key* vending_machine_keys,int key_number){
    PRINTF_SCREEN(0,3,SCREEN_BACKGROUND,SCREEN_INFORMATION_COLOR,"Préparation de votre commande");
    int i=0, x=3;
    article* a;
    for(linked_list* l=basket->next;l!=NULL;l= l->next){
        for(int y=32,c; y<37; y++){
            a=l->element;
            PRINTF(x, y-1,LEFT_SIDE_COLOR,GREY,"  ");
            PRINTF(x, y,LEFT_SIDE_COLOR,GREY,"%.4s\n",a->picture);
            PRINTF_SCREEN(29+i%3,3,SCREEN_BACKGROUND,SCREEN_INFORMATION_COLOR,".  ");
            i++;
            usleep(700*1000);
        }
        x+=4;
    }
    PRINTF_SCREEN(0,6,SCREEN_BACKGROUND,SCREEN_INFORMATION_COLOR,"Veuillez récupérer vos articles");
    PRINTF_SCREEN(0,7,SCREEN_BACKGROUND,SCREEN_WAIT_COLOR,"Pressez [Entrer] pour quitter.");
    while (get_vending_machine_key(vending_machine_keys,key_number)!='\r');
    return (vending_machine_function){NULL};
}

vending_machine_function add_element(linked_list* basket,key* vending_machine_keys,int key_number){
    FILL_SCREEN(SCREEN_BACKGROUND);
    PRINTF_SCREEN(0,0,SCREEN_BACKGROUND,SCREEN_INPUT_TITLE_COLOR,"saisissez le code de l'article:");
    PRINTF_SCREEN(0,2,SCREEN_BACKGROUND,SCREEN_HIGHLIGHT_COLOR,"Pressez [Echap] pour annuler");
    char input[4],type_article;
    unsigned int number;
    article* a;
    if (screen_gets(input,4,vending_machine_keys, key_number,31,0)!=NULL){
        FILL_SCREEN(SCREEN_BACKGROUND);
        if(sscanf(input,"%c%u",&type_article, &number)==2){
            int isvalue=-1;
            for (int i=0;i<3;i++) if (product_list[i]->name[0]==type_article) isvalue=i;
            if (isvalue!=-1){
                a=linked_list_get_element(product_list[isvalue]->article_list,number);
                if (a!=NULL){
                    if (linked_list_get_lenght(basket)<10){
                        if (linked_list_get_range(basket,a)==-1){
                            basket=linked_list_add_element(basket,a);
                            return (vending_machine_function){main_menu};
                        }else{
                            PRINTF_SCREEN(0,0,SCREEN_BACKGROUND,SCREEN_ERROR_COLOR,"Cet article est déjà dans le panier\n");
                        }
                    }else{
                        PRINTF_SCREEN(0,0,SCREEN_BACKGROUND,SCREEN_ERROR_COLOR,"Le panier est plein!\n");
                    }
                }else{
                    PRINTF_SCREEN(0,0,SCREEN_BACKGROUND,SCREEN_ERROR_COLOR,"Aucun article n°%d dans %.15s\n",number,product_list[isvalue]->name);
                }
            }else{
                PRINTF_SCREEN(0,0,SCREEN_BACKGROUND,SCREEN_ERROR_COLOR,"Catégorie d'article inconnu!\n");
            }
        }else{
            PRINTF_SCREEN(0,0,SCREEN_BACKGROUND,SCREEN_ERROR_COLOR,"Erreur dans la saisie!\n");
        }
        screen_wait_message(3,1);
    }
    return (vending_machine_function){main_menu};
}

char* screen_gets(char* str,int max_character,key* vending_machine_keys,int key_number,int x,int y){ //fgets
    if (str!=NULL){
        memset(str,'\0',max_character);
        for(int c,i=0;;){
            c=get_vending_machine_key(vending_machine_keys,key_number);
            if (c=='\r'){
                return str;
                break;
            }else if (c=='\e'){
                return NULL;
                break;
            }else if (i<max_character-1 && c!=-1){
                str[i]=c;
                PRINTF_SCREEN(x+i,y,SCREEN_BACKGROUND,SCREEN_INPUT_TEXT_COLOR,"%c",c);
                i++;
            }
        }
    }else{
        return NULL;
    }
}

void screen_wait_message(int time,int y){
    for (int i=0;i<time;i++){
        PRINTF_SCREEN(0,y,SCREEN_BACKGROUND,SCREEN_WAIT_COLOR,"Veuillez patienter %d seconde(s)\n",time-i);
        sleep(1);
    }
}

void display_key(key k,unsigned int bg,unsigned int fg){
    fill_surface(k.x,k.y,k.x+5,k.y+2,bg);
    switch (k.code){
        case '\e':PRINTF(k.x,k.y+1,bg,fg,"echap\n");break; //escape
        case '\r':PRINTF(k.x,k.y+1,bg,fg,"entrer\n");break; //carriage return
        default:PRINTF(k.x+3,k.y+1,bg,fg,"%c\n",k.code); //default case
    }
}

int get_vending_machine_key(key* vending_machine_keys,int key_number){
    int j=-1;
    for (int i=0,c=upper(getchar());i<key_number;i++){
        if (c==vending_machine_keys[i].code){
            j=c;
            putchar('\a'); //beep
            display_key(vending_machine_keys[i],KEYS_HIGHLIGHT_BACKGROUND,KEYS_HIGHLIGHT_FOREGROUND);
            usleep(KEYS_HIGHLIGHT_TIME*1000); //sleep 150ms (150 000us) fontion usleep dans unistd.h
            display_key(vending_machine_keys[i],KEYS_BACKGROUND,KEYS_FOREGROUND);
            break;
        }
    }
    return j;
}

void display_product_class(product_class* p,unsigned int x,unsigned int y,int dimx,unsigned int dimy){
    if (p!=NULL){ //si le pointer est valable
        int i=0,max=dimx*dimy; //maximimum d'article à afficher
        fill_surface(x,y,x+7*dimx,y+4*dimy,PRODUCT_CLASS_BACKGROUND); //remplissage du cadre sur lequel est affiché les articles
        PRINTF(x+1,y,PRODUCT_CLASS_BACKGROUND,PRODUCT_CLASS_FOREGROUND,"%s",p->name);
        for (linked_list* l=p->article_list;l!=NULL && i<max;l=l->next){ //parcours de la liste chainé.
            display_article(l->element,x+7*(i%dimx)+1,y+4*(i/dimx)+1); //affichage de l'article
            i++;
        }
    }
}

void display_article(article* a,unsigned int x,unsigned int y){
    fill_surface(x,y,x+5,y+2,ARTICLE_BACKGROUND);
    PRINTF(x+2,y,ARTICLE_BACKGROUND,ARTICLE_FOREGROUND,"%.4s",a->picture); //affiche l'émoticone
    PRINTF(x+1,y+1,ARTICLE_BACKGROUND,ARTICLE_FOREGROUND,"%.1f€",a->price); //affiche 1.1€
    PRINTF(x+1,y+2,ARTICLE_BACKGROUND,ARTICLE_FOREGROUND,"%.3s",a->code); //affiche par exemple f00
}

product_class* create_new_product_class(char* file_name){ //char* chaine de caractère
    FILE* fp=fopen(file_name,"r"); //ouverture en mode reading
    if (fp!=NULL){
        char picture[4],name[15];
        float price;
        article* a;
        product_class* p=malloc(sizeof(product_class));
        p->article_list=NULL;
        strncpy(p->name,file_name,15);
        for (int i=0;fscanf(fp,"%4s %f %15s",picture,&price,name)==3 && i<100;i++){ //parcours les lignes du fichier.(scanf attend un retour à la ligne)
            a=malloc(sizeof(article));
            a->code[0]=file_name[0]; //ex:f pour fruits
            a->code[1]='0'+i/10; //1 des dizaines
            a->code[2]='0'+i%10; //1 des unités
            a->price=price;
            strncpy(a->picture,picture,4); //copie de l'émoticone
            strncpy(a->name,name,15); //copie du nom
            p->article_list=linked_list_add_element(p->article_list,a); //ajoute l'article dans la liste
        }
        fclose(fp); //ferme le fichier
        return p; //retourne la classe de produit créée
    }else{
        return NULL; //en cas d'échéance retourne le pointer NULL
    }
}

int linked_list_get_lenght(linked_list* l){
    int lenght = 0;
    for(;l!=NULL; l=l->next) lenght++;
    return lenght;
}

int linked_list_get_range(linked_list* l, void* element){
    int return_value = -1;
    if(l!=NULL){
        for(int i=0;l!=NULL;l=l->next){
            if(l->element==element){
                return_value=i;
                break;
            }
            i++;
        }
    }
    return return_value;
}

void* linked_list_get_element(linked_list* l, unsigned int range){
    if(l!=NULL){
        void* return_value = NULL;
        for(int i=0; l!=NULL; l=l->next){
            if(i==range){
                return_value=l->element;
                break;
            }
            i++;
        }
        return return_value;
    }else {
        return NULL;
    }
}

linked_list* linked_list_remove_range(linked_list* l,int range){
    if (l!=NULL){
        if (range==0){
            free(l);
            return l->next;
        }else{
            linked_list* lc=NULL;
            for (int i=1;l->next!=NULL;l=l->next){
                if (i==range){
                    lc=l->next;
                    l->next=l->next->next;
                    free(lc);
                    lc=l;
                    break;
                }
                i++;
            }
            return lc;
        }
    }else{
        return NULL;
    }
}

linked_list* linked_list_add_element(linked_list* ll,void* element){ //rend en argument le pointer du premier bloc de la liste chainé et le pointer de l'élément à l'intérieur du bloc;
    if (ll==NULL){ //si aucun bloc n'existe
        ll=malloc(sizeof(linked_list)); //créé le nouveau bloc
        ll->element=element; //attribut un valeur
        ll->next=NULL; //pas de bloc suivant donc on attribut l'adresse NULL
    }else{ //si il existe au moins un bloc
        linked_list* lli=ll;
        while (lli->next!=NULL) lli=lli->next; //tant que le bloc suivant existe, on change la valeur de lli à celle du bloc suivant
        lli->next=malloc(sizeof(linked_list)); //on met dans le bloc précédent l'adresse du bloc suivant qui vient d'être créé.
        lli->next->element=element; //on ajoute l'adresse à stocker dans le nouveau bloc.
        lli->next->next=NULL; //dans le nouveau bloc créé on met l'adresse du bloc suivant à NULL
    }
    return ll;
}

void free_linked_list(linked_list* l,bool free_elements){
    if (l!=NULL){
        for (linked_list* lc;l!=NULL;){
            if (free_elements) free(l->element);
            lc=l;
            l=l->next;
            free(lc);
        }
    }
}

void fill_surface(int x1 ,int y1,int x2,int y2,int color){
    for (;y1<=y2;y1++){ //parcours colonne
        for (int x=x1;x<=x2;x++){ //création d'une variable locale x et parcours d'une ligne
            PRINTF(x,y1,color,color," "); //position couleur reset
        }
    }
}

int upper(int c){
    if (c>='a' && c<='z') c=c-'a'+'A';
    else{
        switch (c){
            case '&':c='1';break;
            case 169:c='2';break; //é
            case '"':c='3';break;
            case '\'':c='4';break;
            case '(':c='5';break;
            case '-':c='6';break;
            case 168:c='7';break; //è
            case '_':c='8';break;
            case 167:c='9';break; //ç
            case 160:c='0'; //à
        }
    }
    return c;
}
