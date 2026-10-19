#ifndef ADMIN_H
#define ADMIN_H

#include "items.h"

#define MAX_KEYWORDS 50
#define MAX_FRAUDS 200
#define ADMIN_FILE "admin.txt"
#define ADMIN_PIN "1234"

#define FRAUD_PENDING 1
#define FRAUD_VERIFIED 2
#define FRAUD_DISMISSED 3

typedef struct {
    int id;
    int itemId;
    int status;
    char suspectName[60];
    char accuserName[60];
    char accuserContact[60];
    char reason[200];
    char date[11];
} FraudReport;

typedef struct {
    int kwCount;
    char keywords[MAX_KEYWORDS][40];
    int fraudCount;
    FraudReport frauds[MAX_FRAUDS];
} AdminData;

void initDefaultAdminData(AdminData *admin);
void setActiveAdminData(AdminData *admin);
const char *detectBlacklistedKeyword(const Item *item);

/* Admin item management & blacklist operations */
int updateItem(Item items[], int count, const AdminData *admin);
int deleteItem(Item items[], int *count);
int reviewBlacklistedItems(Item items[], int *count);
int manageBlacklist(AdminData *admin, Item items[], int count);

/* Fraud allegation reporting & admin review */
int submitFraudAllegation(const Item items[], int count, AdminData *admin);
int reviewFraudAllegations(Item items[], int count, AdminData *admin);

#endif