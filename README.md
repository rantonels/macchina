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

Si interagisce col programma con singoli keystrokes.

`R`:  reimposta la plancia nella situazione iniziale

`T`:  calcola ed esegui una mossa per il giocatore inferiore. I punteggi stimati e le mosse possibili vengono visualizzati progressivamente.

`Y`: calcola ed esegui una mossa per il giocatore superiore.

`S`/`D`: diminuisce/aumenta la profondità di ricerca (default: 5). Questo è il numero di mosse che il programma è in grado di prevedere nel futuro. Il range di valori possibili è 2-11.

`K`: svolgi una partita fra due copie del programma, partendo dalla situazione iniziale e con il turno al giocatore inferiore.

`F`: scambia i colori dei due giocatori (non cambia nulla ai fini dei calcoli o dei turni, è esclusivamente estetica).

`M`: esegui una mossa con il giocatore superiore. Una volta premuto `m`, verrà presentato un menù con le mosse possibili. Scegliere con `su`/`giù` e accettare con `enter`, annullare con `q`. Per proporre una patta parallelamente alla mossa, premere il tasto `p` prima di accettarla.

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

`--openings, -o` genera/rigenera il database delle seconde mosse. Di default questo è fatto a profondità 11 se non è specificata l'opzione -d. Il database è salvato in `db/database` e ricaricato a inizio esecuzione della GUI. Il programma usa il database per evitare di calcolare mosse inutilmente.

`--depth, -d n` imposta la profondità di ricerca ad n mosse in avanti nelle ricerche in linea di comando (nella GUI invece si imposta manualmente con `S`/`D`). Notare che una profondità inferiore a 12 rende poco utile il precalcolo delle mosse.
