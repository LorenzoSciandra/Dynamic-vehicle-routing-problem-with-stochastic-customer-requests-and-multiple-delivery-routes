# Dynamic vehicle routing problem with stochastic customer requests and multiple delivery routes
![LICENSE SHIELD](https://img.shields.io/badge/license-GPL3-red)

Questo progetto contiene l'analisi teorica e l'implementazione di un problema di vehicle routing di tipo same-day delivery del professor [Jean-François Côté](https://www4.fsa.ulaval.ca/enseignant/jean-francois-cote/). Quest'ultimo, insieme ai suoi colleghi, ha implementato il **Branch & Regret**, un algoritmo euristico di cui il mio compagno Stefano Vittorio Porta ed io abbiamo investigato le proprietà attraverso un algoritmo esatto, un **Branch & Bound**. Il problema analizzato si presenta oggigiorno in moltissimi contesti reali, soprattutto per quanto riguarda il settore degli e-commerce online, per garantire la consegna di più ordini nello stesso giorno in cui sono stati richiesti: **same day deliver problem** (es: Amazon Prime, Uber Eats, Glovo, etc.). 
Questa tesina è stata realizzata come progetto d'esame per il corso magistrale Modelli e Metodi per il Supporto alle Decisioni presso [Unito](https://www.unito.it/).

## Documentazione
Per approfondire il problema teorico e le idee sottostanti l'implementazione di Jean-François Côté e la nostra si consiglia di visionare il file [Documentazione](./documentation/Paper.pdf).

## Esecuzione
Per eseguire il codice consigliamo, per semplicità, di eseguire in ordine una serie di script python che abbiamo realizzato:
1. `runner.py`: seleziona quanti e quali istanze di problema eseguire (default=810) e genera altrettanti files di output .txt contenenti i risultati. Nello specifico saranno generati due file per ogni istanza, dato che queste saranno eseguite sia con il Branch & Bound che con il Branch & Regret;
2. `extractor.py`: accorpa tutte le informazioni rilevanti contenute nei file generati con l'esecuzione precedente in due tabelle .csv una per il Branch & Bound e l'altra per il Branch & Regret;
3. `analyzer.py`: genera in maniera interattiva da terminale diversi plot di confronto sui risultati ottenuti.


## Disclaimer

> The primary authors of the code are Professor Jean Francois Cote and his colleagues, who are researching these issues. Please note that the code may not be in its final version and therefore may not be suitable for commercial use. As indicated by the headers of the files, it is intended for academic purposes only. If you have any suggestions or would like to propose improvements, please contact Jean-Francois Cote at jean-francois.cote@fsa.ulaval.ca.