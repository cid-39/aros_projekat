Projekat na predmetu "Arhitekturi racunara i operativnih sistema"

Tema projekta: Program za deljenje fajlova izmedju procesa implementiran u programskom jeziku C upotrebom deljenje memorije i semafora 


Princip rada:

Pokrece se daemon koji ceka da mu se javi odrednjeni filepasser sa nazivom fajla ili -s opcijom za gasenje daemona, kada filepasser posalje ime fajla, oba procesa pokrecu nove procese, filepasser client-a, a daemon server-a. Kada se obavi razmena fajla, gase se deca procesi, filepasser ceka da se zavrsi client.
