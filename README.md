In functia main tin minte al doilea argument din linia de comanda si il compar
cu task-ul pe care trebuie sa il realizez, apeland functiile corespunzatoare.

Pentru construirea retelei de sateliti am implementat toate functiile clasice
caracteristice unui heap: initializare, minheap, inserare, extragere si o
functie care interschimba elementele heap-ului. In functia de construire a
arborelui binar, citesc toti satelitii cu frecventa si numele lor, si ii adaug
in arbore 2 cate 2 respectand proprietatile din enunt, la final returnand
radacina arborelui si eliberand heap-ul.

Pentru a afisa arborele pe niveluri, am implementat o coada
alocata dinamic cu cele 2 functii de push si pop, si in functia BFS care aplica
algoritmul cu acelasi nume, cat timp coada nu este vida, elimin nodul curent
din coada si adaug nodurile din stanga si dreapta nodului curent in coada.
Acest lucru il fac de nr ori, unde nr reprezinta numarul de noduri de pe
nivelul curent(mereu putere a lui 2 din care scadem 1), eliberand in acelasi
timp nodurile arborelui, urmand ca la final sa eliberez si coada.

Pentru task-urile 2, 3 si 4 voi apela functia care imi construieste arborele
utilizata la cerinta anterioara si la final voi elibera arborele recursiv,
folosind functia free_tree.

La task-ul 2, pentru fiecare codificare ma misc prin arbore in stanga sau in
dreapta pana cand ajung la o frunza, caz in care afisez nodul corespunzator
codificarii binare si ma reintorc la radacina arborelui.

La cea de-a treia cerinta, pentru a determina codificarea binara
corespunzatoare fiecarui satelit, aloc dinamic codificarea si parcurg recursiv
nodurile arborelui astfel : daca am ajuns la un nivel mai mare decat memoria
pe care am alocat-o initial codificarii, atunci marim memoria, iar daca am
gasit numele satelitului, pun terminatorul la finalul codificarii si o
returnez. In caz contrar, aplic algoritmul de DFS si actualizez codificarea cu
0 sau 1 in functie de nodul in care ma aflu, iar in final eliberez codificarea.

La task-ul 4 aloc dinamic toti satelitii cititi din fisier si determin recursiv
cel mai apropiat parinte comun primilor 2 sateliti. Dupa aceea, determin 
recursiv cel mai apropiat parinte comun satelitului determinat anterior si 
satelitului curent si tot asa. La final se va afla parintele tuturor 
satelitilor si il voi afisa, dupa care eliberez sirul de sateliti.

Pentru task-ul 5, refac toate functiile de la task-ul 1 pentru a se potrivi
pe noua structura care poate retine mai mult de 2 noduri ca si copii. Astfel,
arborele principal il voi construi la fel doar ca voi folosi decat 2 copii
pentru fiecare nod, iar arborii multicai pot avea si mai multi.

Functia search_multitree cauta recursiv prin tot arborele pana gaseste numele
nodului cautat.

Functia care construieste restul arborelui (adaugarea arborilor multicai) va
aloca dinamic pentru fiecare arbore multicai numele satelitului de care se
leaga si cauta recursiv prin tot arborele unde se afla nodul cu numele dat,
dupa care elibereaza numele. In continuare, am alocat dinamic radacina fiecarui
arbore multicai si i-am asociat numele si frecventa, eliberand numele temporar.
Totodata, am legat radacina arborelui multicai de satelitul din arborele binar
,alocand memorie pentru copilul arborelui binar. Dupa aceea, citesc numarul de
noduri parinte al arborelui multicai, aloc dinamic numele fiecarui parinte si
caut prin tot arborele nodul corespunzator, eliberand din memorie numele
temporar. In continuare, citesc numarul de copii ai parintelui curent, realoc
memorie pentru vectorul de copii ai arborelui parinte, aloc memorie pentru
fiecare copil al parintelui curent si asociez fiecaruia frecventa si numele din
fisierul de intrare. In acelasi timp, aloc memorie pentru copii fiecarui copil
al parintelui curent, initializandu-l pe primul ca fiind NULL. La final, nu uit
ca dupa ce am linkat toti copiii nodului parinte, mai initializeza inca un
copil care sa fie NULL.

In functia task5 citesc cele 2 nume pentru care trebuie sa determin distanta
si caut in tot arborele nodurile corespunzatoare numelor. Dupa aceea, caut
recursiv cel mai mic parinte comun celor 2 noduri si il returnez, dupa care
determin distantele de la el la cele 2 noduri si afisez suma lor. La final,
eliberez recursiv toate numele si toate nodurile din arbore.