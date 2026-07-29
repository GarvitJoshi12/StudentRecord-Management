#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Student {
    int roll;
    char name[50];
    char dept[20];
    int sem;
    float cgpa;
    float attendance;
    struct Student *next;
};

struct Student *head = NULL;

void reassignRolls() {
    int r = 1;
    struct Student *t = head;
    while (t) {
        t->roll = r++;
        t = t->next;
    }
}

void saveToFile() {
    FILE *fp = fopen("students.txt", "w");
    if (!fp) { printf("File error!\n"); return; }

    fprintf(fp, "%-5s %-20s %-15s %-5s %-6s %-8s\n",
            "ROLL", "NAME", "DEPT", "SEM", "CGPA", "ATT%");
    fprintf(fp, "------------------------------------------------------------------------------\n");

    struct Student *t = head;
    while (t) {
        fprintf(fp, "%-5d %-20s %-15s %-5d %-6.2f %-8.2f\n",
                t->roll, t->name, t->dept, t->sem, t->cgpa, t->attendance);
        t = t->next;
    }
    fclose(fp);
}

void loadFromFile() {
    FILE *fp = fopen("students.txt", "r");
    if (!fp) return;

    char line[200];
    fgets(line, sizeof(line), fp); 
    fgets(line, sizeof(line), fp); 

    while (fgets(line, sizeof(line), fp)) {
        struct Student *s = (struct Student *)malloc(sizeof(struct Student));
        if (sscanf(line, "%d %s %s %d %f %f",
                   &s->roll, s->name, s->dept,
                   &s->sem, &s->cgpa, &s->attendance) != 6) {
            free(s);
            continue;
        }
        s->next = NULL;
        if (!head || strcasecmp(s->name, head->name) < 0) {
            s->next = head;
            head = s;
        } else {
            struct Student *t = head;
            while (t->next && strcasecmp(t->next->name, s->name) < 0)
                t = t->next;
            s->next = t->next;
            t->next = s;
        }
    }
    fclose(fp);
    reassignRolls();
}

void sortedInsert(struct Student *s) {
    if (!head || strcasecmp(s->name, head->name) < 0) {
        s->next = head;
        head = s;
    } else {
        struct Student *t = head;
        while (t->next && strcasecmp(t->next->name, s->name) < 0)
            t = t->next;
        s->next = t->next;
        t->next = s;
    }
    reassignRolls();
    saveToFile();
}

void addStudent() {
    struct Student *s = (struct Student *)malloc(sizeof(struct Student));
    printf("Enter Name: "); scanf("%s", s->name);
    printf("Enter Dept: "); scanf("%s", s->dept);
    printf("Enter Sem: "); scanf("%d", &s->sem);
    printf("Enter CGPA: "); scanf("%f", &s->cgpa);
    printf("Enter Attendance: "); scanf("%f", &s->attendance);
    s->next = NULL;
    sortedInsert(s);
    printf("Student Added Successfully!\n");
}

void displayStudents() {
    if (!head) { printf("No records found!\n"); return; }
    printf("\n%-5s %-20s %-15s %-5s %-6s %-8s\n",
           "ROLL","NAME","DEPT","SEM","CGPA","ATT%");
    printf("--------------------------------------------------------------------------\n");
    struct Student *t = head;
    while (t) {
        printf("%-5d %-20s %-15s %-5d %-6.2f %-8.2f\n",
               t->roll,t->name,t->dept,t->sem,t->cgpa,t->attendance);
        t = t->next;
    }
}

void searchStudent() {
    int roll; printf("Enter Roll No: "); scanf("%d",&roll);
    struct Student *t = head;
    while (t) {
        if (t->roll == roll) {
            printf("Found: %s | %s | SEM %d | CGPA %.2f | ATT %.2f%%\n",
                   t->name, t->dept, t->sem, t->cgpa, t->attendance);
            return;
        }
        t = t->next;
    }
    printf("Record not found!\n");
}

void deleteStudent() {
    int roll; printf("Enter Roll to delete: "); scanf("%d",&roll);
    struct Student *t=head,*p=NULL;
    while(t && t->roll!=roll){ p=t; t=t->next; }
    if(!t){ printf("Record not found!\n"); return; }
    if(!p) head=t->next; else p->next=t->next;
    free(t);
    reassignRolls();
    saveToFile();
    printf("Record Deleted!\n");
}

void updateStudent() {
    int roll; printf("Enter Roll to update: "); scanf("%d",&roll);
    struct Student *t=head;
    while(t && t->roll!=roll) t=t->next;
    if(!t){ printf("Record not found!\n"); return; }

    char newName[50], newDept[20]; int newSem; float newCgpa,newAtt;
    printf("New Name: "); scanf("%s", newName);
    printf("New Dept: "); scanf("%s", newDept);
    printf("New Sem: "); scanf("%d",&newSem);
    printf("New CGPA: "); scanf("%f",&newCgpa);
    printf("New Attendance: "); scanf("%f",&newAtt);

    deleteStudent(); 
    struct Student *s=(struct Student *)malloc(sizeof(struct Student));
    strcpy(s->name,newName); strcpy(s->dept,newDept);
    s->sem=newSem; s->cgpa=newCgpa; s->attendance=newAtt; s->next=NULL;
    sortedInsert(s);

    printf("Record Updated Successfully!\n");
}

void meritList() {
    if(!head){ printf("No records!\n"); return; }
    int n=0; struct Student *t=head;
    while(t){ n++; t=t->next; }
    struct Student **arr = (struct Student**)malloc(n*sizeof(struct Student*));
    t=head; int i=0;
    while(t){ arr[i++]=t; t=t->next; }

    for(i=0;i<n-1;i++)
        for(int j=i+1;j<n;j++)
            if(arr[i]->cgpa < arr[j]->cgpa){
                struct Student *tmp=arr[i]; arr[i]=arr[j]; arr[j]=tmp;
            }

    printf("\nRANK  NAME            DEPT       CGPA\n");
    printf("--------------------------------------------\n");
    for(i=0;i<n;i++)
        printf("%-5d %-15s %-10s %-6.2f\n", i+1, arr[i]->name, arr[i]->dept, arr[i]->cgpa);

    free(arr);
}

void eligibilityList() {
    float cg,at;
    printf("Minimum CGPA: "); scanf("%f",&cg);
    printf("Minimum Attendance: "); scanf("%f",&at);

    struct Student *t=head;
    int found=0;
    printf("\nROLL  NAME            CGPA   ATT%\n");
    printf("---------------------------------------\n");
    while(t){
        if(t->cgpa>=cg && t->attendance>=at){
            printf("%-5d %-15s %-6.2f %-6.2f\n",
                   t->roll,t->name,t->cgpa,t->attendance);
            found=1;
        }
        t=t->next;
    }
    if(!found) printf("No eligible students found!\n");
}

void statistics() {
    if(!head){ printf("No data!\n"); return; }
    struct Student *t=head; float sum=0,max=0,min=10; int c=0;
    while(t){ sum+=t->cgpa; if(t->cgpa>max) max=t->cgpa; if(t->cgpa<min) min=t->cgpa; c++; t=t->next; }
    printf("Average CGPA: %.2f\nHighest CGPA: %.2f\nLowest CGPA : %.2f\n",sum/c,max,min);
}

int main(){
    loadFromFile();
    int ch;
    do{
        printf("\n1.Add\n2.Display\n3.Search\n4.Delete\n5.Update\n6.Merit\n7.Eligibility\n8.Statistics\n9.Exit\nChoose: ");
        scanf("%d",&ch);
        switch(ch){
            case 1: addStudent(); break;
            case 2: displayStudents(); break;
            case 3: searchStudent(); break;
            case 4: deleteStudent(); break;
            case 5: updateStudent(); break;
            case 6: meritList(); break;
            case 7: eligibilityList(); break;
            case 8: statistics(); break;
            case 9: saveToFile(); printf("Exit & Saved!\n"); break;
            default: printf("Invalid choice!\n");
        }
    }while(ch!=9);
    return 0;
}
