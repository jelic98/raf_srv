### Server (FreeRTOS Scheduler)

* Dodati compute i period u tcb
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
