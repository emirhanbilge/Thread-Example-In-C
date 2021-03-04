#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/*  Mantığı  / Logic 

    Unitler kendi içlerine while ile sürekli person almaya çalışacaklar fakat başlangıçta unnitler aktif değil
    1- UnitLock semaphoru  : Bu semaphore person oluştuğunda ve globalID'yi değiştirdiği zaman unitleri aktif etmekle görevlidir.
    2- PersonLock semaphoru: Bu semaphore ise personlar thread ve eş zamanlı olduğundan her biri globalID'yi o gelen personun ID'si yapmak
                    isteyecektir.Bunu engellemek için personların global dataya erişimini düzenler.
    3- Control   semaphoru : Person global Dataya eriştikten sonra Unitlerden biri bunu tüketmedikçe globalID'nin değişmemesi gerekir
                    Personlar da dahil bu globalID'yi değiştirmemelidir. Person Üretim yaptıktan sonra Uniti Aktif etmekte 
                    Unit ise bu semaphore vasıtası ile tamam kullandım , person yenisini üretebilir demektedir.Asıl senkronu yapan
                    semaphorelardan biridir. Control Person tarafından kilitlenip ID değiştirilir Unit aktif olup ID'yi kullanır ve
                    personu tekrar aktif eder. Mekanizmanın temeli budur. Local semaphore ise Thredimdeki 'STAFF' isminde if elseleri
                    düzgün yapması ve printleri düzgün bastırması amacıyla konulmuştur.

    Units will try to get person in themselves with while, but initially unnits are not active.
    1- UnitLock semaphore: This semaphore person is responsible for activating the units when it occurs and changes the globalID.
    2- PersonLock: In this semaphore, since the personas are thread and synchronous, each one will want to make the globalID as the ID of that 
                incoming person. In order to prevent this, it regulates the access of persons to global data.
    3- Control: After the Person accesses the global Data, the globalID should not change unless one of the Units consumes it. This globalID,
                 including the Person, should not change. Person activates the unit after manufacturing, and the Unit says that I have used it through this 
                semaphore, the person can produce a new one. It is locked by Control Person and the ID is changed. The unit is active, uses the ID and reactivates 
                 the personu. This is the basis of the mechanism. The local semaphore is in the name of 'STAFF' in Thredim to make if hands properly and print prints properly.
    

                                                                                            Emirhan Bilge Bulut (EBB)  05.01.2020 
*/

#define HowManyPeople 150 // Number of People
#define HowManyUnit 8     // Number of Units
int globalID;             // Common data Person-Unit share
sem_t PersonLock;         // The semaphore that provides the lock required for the person to change the globalID (Personun globalID'yi değiştirmesi için gerekli kilidi sağlayan semaphore)
sem_t UnitLock;           // When a unit grabs a person while competing to grab a person from units, other units should not touch that person. (Unitlerden personu kapmak için yarışmakta bir unit kişiyi kaptı mı diğer unitler o kişiye dokunmamalı onu sağlayan semaphore)
sem_t Control;            /* The semaphore required for the proper data exchange between Person and Unit. Once the person changes the data, another person should not change it until the Unit receives it. 
                            Person ile Unitin arasındaki data alış verişini sağlıklı yürütmesi için gereken semaphore Person datayı değiştirdi mi
                            Unit onu alana kadar bir başka person onu değiştirmemeli*/
void *Person(void *Number)
{
    int r = rand() % 500;
    usleep(r);
    printf("Person %d came to hospital \n", *((int *)Number));

    sem_wait(&Control);    //Here, the moment the control locks, the units are immediately cut off from accessing the globalID and they do not work. Burada control kilitlediği anda unitler an itibari ile globalID'ye erişimleri kesilmiş oluyor ve çalışmıyorlar.
                           //The non-working part of the units is not the whole, but only the parts that access globalID and make its operations. Unitlerin çalışmayan kısmı tamamı değil sadece globalID ye erişip onun işlemlerini yaptığı kısımlar
    sem_wait(&PersonLock); //To prevent other personas from changing the globalID, here to avoid person threads - Diğer personlar globalID 'yi değiştirmesini engellemek için burada person threadlerinin önüne geçmek için
    globalID = *((int *)Number);
    sem_post(&PersonLock);
    sem_post(&UnitLock); //Units cannot run after any person arrives, triggering the operation of the unit. - Herhangi bir person gelmedikten sonra unitlerin çalışması olamaz unitin çalışmasını tetikliyor
}

void *Unit(void *Number)
{
    int UnitID = *((int *)Number);
    int people[3];       //Threads must have a local variable to keep 3 incoming people. It doesn't make sense to keep it global. -  3 tane gelen insanı tutması için threadlerin local değişkeni olmalı globalde tutmak mantıklı değil
    int size = 0;        //Both as an index when placing it in arrey and in terms of telling how many people are-  Hem arreye yerleştirirken index olarak hem de kaç kişinin olduğunu söylemesi açısından
    sem_t StaffAnnounce; // This semaphore is not very necessary, but I put it in the control phase to prevent another thread printing from being abandoned. - Bu semaphore çok gerekli değil fakat kontrol aşamasında başka bir thread printi yarım bıraktırmasın diye koydum
    sem_init(&StaffAnnounce, 0, 1);
    while (1)
    {
        sem_wait(&StaffAnnounce);
        if (size == 0 || size == 1)
        {
            printf("Unit %d has %d free space\n", UnitID, (3 - size));
        }
        if (size == 2)
        {
            printf("Staff Announced : Last people is waited by Covid-19 test unit %d !!! Please, pay attention to your social distance and hygiene; use a mask \n", UnitID);
        }
        printf("\n");
        sem_post(&StaffAnnounce);

        sem_wait(&UnitLock); //Here, as soon as the person increases the Unitlock, one unit will access global data, whichever comes first, it will lock itself. - Buradaki kısım person Unitlocku arttırdığı anda bir unit global dataya erişecek hangisi önce gelirse kendisini kilitlicek
        if (size < 3)
        {
            /*The if else mechanism here exists because of this, if I put sleep, that thread could still grab the personu when there were 3 of them in the threade or when the 
            ventilation was not over. We do not know which thread will run first. If it is full and is in the waiting phase, I check you to
            prevent it. If the thread with Size is 3, this process will repeat this time until the unit is unlocked with the else and the correct 
            and empty unit (Buradaki if else mekanizması şu yüzden var , eğer sleep koysaydım threade içinde 3 tanesi varken ya da
            havalandırma durumu bitmezken o thread gene personu kapabilirdi. Hangi threadin önce çalışıcağını bilmiyoruz.
            Eğer içi dolu ve bekleme aşamasında kapsaydı bunu önlemek için size kontrolü yapıyorum.Eğer Size'ı 3 olan thread
            kapsaydı gene bu defa else ile Unitin kilidi açılarak doğru ve boş unite girene kadar bu işlem tekrar edecekti)*/

            printf("Person %d entered the waiting room in unit %d.\n", globalID, UnitID);
            people[size] = globalID;
            size++;
            sem_post(&Control); // Here, I am done with Persona global Data message is given. Burada Persona global Data ile işim tamam mesajı verilmekte
            if (size == 3)
            {
                printf("\nPeople %d %d %d is going to Covid-19 test in Unit %d\n", people[0], people[1], people[2], UnitID);
                size = 0;
                printf("Vantilating Room Unit %d\n\n", UnitID);
                //  sleep(1);
            }
        }
        else
        {
            sem_post(&UnitLock);
        }
    }
}

int main()
{

    pthread_t PersonalThread[HowManyPeople], UnitThreads[HowManyUnit]; // Unit ve person thread arrayi
    sem_init(&PersonLock, 0, 1);
    sem_init(&Control, 0, 1);
    sem_init(&UnitLock, 0, 0);
    int UnitIds[HowManyUnit];     // Eğer bu tarz id ler arrey şeklinde tanımlanıp içi doldurumadan yollandığında garip değerler alıyor.
    int PersonIds[HowManyPeople]; // Eğer bu tarz id ler arrey şeklinde tanımlanıp içi doldurumadan yollandığında garip değerler alıyor.
    for (int i = 0; i < HowManyPeople; i++)
    {
        PersonIds[i] = i + 1;
    }
    for (int i = 0; i < HowManyUnit; i++)
    {
        UnitIds[i] = i + 1;
    }
    for (int i = 0; i < HowManyUnit; i++)
    {
        pthread_create(&UnitThreads[i], NULL, (void *)Unit, (void *)&UnitIds[i]);
    }
    for (int k = 0; k < HowManyPeople; k++)
    {
        pthread_create(&PersonalThread[k], NULL, (void *)Person, (void *)&PersonIds[k]);
    }
    for (int i = 0; i < HowManyPeople; i++)
    {
        pthread_join(PersonalThread[i], NULL);
    }
    // Cancel yapma sebebimiz hastalar bitince unitler beklemeye devam etmekte sürekli çalışma halindeler
    // Hasta bittiği anda threadlerin kapanması için bu şekilde yaptım while'ı da bitirdim fakat beklemeleri devam etti
    // Bu şekilde hepsini kill ederek işlemler bitince yapma gereği duydum.
    for (int i = 0; i < HowManyUnit; i++)
    {
        pthread_cancel(UnitThreads[i]);
    }
    return 0;
}
