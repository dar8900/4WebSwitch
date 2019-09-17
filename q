[33mcommit 608b61c01e16e506d380382568191d5e1884abc0[m[33m ([m[1;36mHEAD -> [m[1;32mmaster[m[33m, [m[1;31morigin/master[m[33m, [m[1;31morigin/HEAD[m[33m)[m
Author: dar8900 <dar8900@gmail.com>
Date:   Tue Sep 17 16:14:44 2019 +0200

    Chiusura versione 1.1

[33mcommit 6d1493b693fd6252f4beb638851b191ca64bf491[m
Author: dar8900 <dar8900@gmail.com>
Date:   Tue Sep 17 11:10:16 2019 +0200

    - Aggiunti alcuni fix per i disegni dei vari menu
    - Inserita la possibilità di attivare o disattivare la modalità simulazione

[33mcommit 8a358e8175f7d0b9ebda63e1e9338b2f9344968b[m
Author: dar8900 <dar8900@gmail.com>
Date:   Fri Sep 13 16:25:24 2019 +0200

    -Inserito flag per abilitare la modalita demo
    -Fix disegno pagina rele e reset e misure
    -Inserito disegno pagina del setup allarmi
    -Inserito disegno per gestione modalità demo
    -Inserita icona per gestione allarme
    -Inserita icona per disegno delle icone delle prese

[33mcommit 3761286d314d311c1ff30cf5b630b1a10724453c[m
Author: dar8900 <dar8900@gmail.com>
Date:   Thu Sep 12 17:04:42 2019 +0200

    Aggiunto nuovo array dedicato alla ricerca dello 0 per la tensione, aggiunte e aggiornate funzioni per la formattazione del tempo da web, aggiunto campo agli allarmi dedicato alla data (in timestamp) in cui l'allarme è avvenuto, aggiunto in eeprom il salvataggio delle occorrenze degli allarmi

[33mcommit 9ba9db0055385155c0009c304e1b3738c88a95b1[m
Author: dar8900 <dar8900@gmail.com>
Date:   Thu Sep 12 15:05:53 2019 +0200

    Modificato il partitore per le misure di tensione e corretto il calcolo per la potenza attiva, inserito calcolo della media sulle letture analogiche per la tastiera, inserita funzione per il controllo del primo avvio per la EEPROM, diminuito il dalay principale del task display

[33mcommit 24300deb78590403c59c05f3771b084d192258c1[m
Author: dar8900 <dar8900@gmail.com>
Date:   Wed Sep 11 11:37:04 2019 +0200

    Diminuito il gain dell'FPGA dell'ACD per gestire meglio la corrente di picco in arrivo dal TA, modificata la formula per la conversione in Volt della lettura analogica

[33mcommit 6b0c1effaee078a384186a021ff61b4f0cc54208[m
Author: dar8900 <dar8900@gmail.com>
Date:   Wed Sep 11 09:52:55 2019 +0200

    Aggiuti moduli eeprom per gestione salvataggi misure, aggiunta gestione dei rele da parte degli allarme in caso di sovrasoglia, aggiunta funzione per i rele che permette lo switch in uno stato scelto dall'utente

[33mcommit d2cee511ad4fa83dbdeeb9252d133856158d40b3[m
Author: dar8900 <dar8900@gmail.com>
Date:   Tue Sep 10 15:46:21 2019 +0200

    Aggiunta pagina per i reset, aggiunti i reset per le misure associate, aggiunta funzione per pop up, aggiunta funzione per gestione statistiche prese

[33mcommit 1e1da3a54ff3c0b04e465762cbd35a61a3fcb069[m
Author: dar8900 <dar8900@gmail.com>
Date:   Mon Sep 9 16:15:49 2019 +0200

    Inserito file per gestione icona wifi

[33mcommit 210d074f9960b7b77d8e07e0bce0810545cbe4bb[m
Author: dar8900 <dar8900@gmail.com>
Date:   Mon Sep 9 16:14:25 2019 +0200

    Iserita gestione dele display per disegno main screen, misure e stato rele, inseriti allarmi, inserita tastiera, inserite nuove misure

[33mcommit 6ecfef7bb722115ec27fcfd3b25ab06d289d7a6d[m
Author: dar8900 <dar8900@gmail.com>
Date:   Fri Sep 6 09:54:36 2019 +0200

    Inserita funzione per il controllo soglie allarmi nella ChekAlarms

[33mcommit 3f2fb4516681a700411aff0e3c9fcd07168dc18f[m
Author: dar8900 <dar8900@gmail.com>
Date:   Fri Sep 6 09:39:52 2019 +0200

    Commit iniziale con progetto da testare
