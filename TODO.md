### Server (FreeRTOS Scheduler)

* RM algoritam
* vTaskDelete na kraju izvrsavanja taska
* Simulirati compute time zavisno od argumenata (neki koriste vTaskDelay a neki ne)

* Interrupt handler za aperiodicne taskove

* Konfiguracija sistema
* Kreiranje taskova
* Brisanje taskova

* Batch posao
* Provera rasporedivosti
* Racunanje maksimalnog kapaciteta rasporedivog servera za skup periodicnih taaskova

### Client (Python App)

* Konfiguracija sistema
* Batch posao i provera rasporedivosti
* Startovanje taskova (periodicni i aperiodicni)
* Zaustavanje taska
* Prikazivanje grafika stanja sistema kroz vreme

### Scheduler Pipeline

1. Python App (configure system)
2. Arduino App (create tasks)
3. FreeRTOS Scheduler (schedule tasks)
4. Arduino App (forward logs)
5. Python App (show chart)

### Notes

RM Agoritam:
- proveriti da li je skup taskova rasporediv
- sortirati taskove
- pri svakom ticku odrediti koji task treba da ide na izvrsavanje
	- ako trenutni task ima jos da racuna (current - start < compute) onda return
	- inace odabrati sledeci task
	- ako se doslo do kraja niza taskova preci na prvi task
