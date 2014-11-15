La Macchina è un giocatore di dama elettrorobotico con minimax+alpha-beta pruning.

Compilazione
==============

controllare di avere le dipendenze:

```bash
    sudo apt-get install libncurses5-dev libboost-program-options-dev
```

e poi fare cd nel repositorio ed eseguire

```bash
    make
```

Esecuzione
==============

eseguendo 

```bash
    ./drafts 
```

in linea di comando, senza opzioni, si passa subito all'interfaccia.

```
        3           0      d4-b6        Depth: 5
    abcdefgh                            DB size: 8
  8  = = = = 8                          HT accesses: 0
  7 = = = =  7                          0104190A
  6  = = = = 6                          0104190A
  5          5
  4          4
  3 =   = =  3
  2  = = = = 2
  1 = = = =  1
    abcdefgh



 d4-b6
 b6-c5
 c3-d4
```

Lo stato di gioco attuale è mostrato graficamente, sotto il numero di semimosse totali. Nelle due colonne immediatamente a destra ci sono le possibili mosse con i relativi punteggi assegnati quando viene chiamato l'algoritmo (i punteggi non hanno un'interpretazione immediata per un essere umano). Ancora più a destra sono mostrate:

* l'attuale profondità di ricerca
* la dimensione del database in memoria
* le queries totali alla tabella di trasposizione (hashtable) dall'ultimo reset
* i due genomi dei giocatori inferiore e superiore

Sotto la plancia c'è una queue degli ultimi messaggi (fra cui le mosse).

Si interagisce col programma con singoli keystrokes.

`R`:  reimposta la plancia nella situazione iniziale

`T`:  calcola ed esegui una mossa per il giocatore inferiore. I punteggi stimati e le mosse possibili vengono visualizzati progressivamente.

`Y`: calcola ed esegui una mossa per il giocatore superiore.

`S`/`D`: diminuisce/aumenta la profondità di ricerca (default: 5). Questo è il numero di mosse che il programma è in grado di prevedere nel futuro. Il range di valori possibili è 2-11.

`K`: svolgi una partita fra due copie del programma, partendo dalla situazione iniziale e con il turno al giocatore inferiore.

`F`: scambia i colori dei due giocatori (non cambia nulla ai fini dei calcoli o dei turni, è esclusivamente estetica).

`M`: esegui una mossa con il giocatore superiore. Una volta premuto `m`, verrà presentato un menù con le mosse possibili. Scegliere con `su`/`giù` e accettare con `enter`, annullare con `q`. Per proporre una patta parallelamente alla mossa, premere il tasto `p` prima di accettarla.

`E`: modifica manualmente la plancia (non conta come mossa). Selezionare una casella e premere un numero da 0 a 4.

`G`: modifica i genomi dei due giocatori (il primo è il giocatore inferiore, il secondo quello superiore). I genomi determinano la funzione di valutazione.

`C`: pulisci la tabella di trasposizione (hashtable).

`Q`: chiudi il programma.

Come giocare contro il computer (in soldoni)
=============================================

Avviate il programma in linea di comando. Premete `D` finché la depth non è alzata ad un valore dignitoso. Premete `F` finché il colore del computer non sia quello inferiore. Dopodiché:

* al turno dell'umano: premete `M` e selezionate la mossa eseguita dall'umano.
* al turno del computer: premete `T` e attendete la mossa del computer.

Se in certi momenti della partita il computer è troppo lento, potete premere `S` per abbassare la depth.


Altre opzioni in linea di comando
====================================

```bash
	./drafts --help
```

mostra le opzioni disponibili.

`--gui, -g` è equivalente a nessuna opzione e apre la GUI.

`--depth, -d n` imposta la profondità di ricerca ad n mosse in avanti nelle ricerche in linea di comando (nella GUI invece si imposta manualmente con `S`/`D`). Notare che una profondità inferiore a 12 rende poco utile il precalcolo delle mosse. 

`--openings, -o` genera/rigenera il database delle seconde mosse. Di default questo è fatto a profondità 11 se non è specificata l'opzione -d. Il database è salvato in `db/database` e ricaricato a inizio esecuzione della GUI. Il programma usa il database per evitare di calcolare mosse inutilmente. Ad esempio, se digitate

```bash
	./drafts --openings -d 12
```

e andate a farvi un tè, al vostro ritorno il database sara aggiornato con le aperture esaminate a profondità 12. Se tutto va bene, la prossima volta che avviate il programma eseguirà le prime due mosse istantaneamente (e meglio).

`--evolution` questa opzione sperimentale permette di evolvere una popolazione di funzioni di valutazione che vengono fatte combattere l'una contro l'altra. Vengono applicati mutazioni e crossover per produrre le generazioni successive.

La dama galileiana
======================

La dama galileiana ufficiale è un misto della dama inglese e di quella italiana. I tratti essenziali sono:

- La plancia viene posizionata con l'angolo nero in basso a sinistra, e si gioca sulle caselle nere.
- Le pedine mangiano sempre e solo in avanti, e non mangiano le dame.
- Quando un giocatore può mangiare, deve farlo (non esiste il *soffio*). Non è costretto a massimizzare il numero di pedine mangiate né con cosa mangiare. Però è tenuto a mangiare finché può.
- Eseguita la centesima mossa, la partita termina. Si contano le pedine (le dame valgono doppio) e chi è in vantaggio vince. Se i punteggi sono uguali è patta.
- Se un giocatore non ha più mosse disponibili, è patta (non persa).
