
typedef struct sbuffer tbuffer;

tbuffer* iniciabuffer (int numpos, int numprod, int numcons);

void deposita (tbuffer* buffer, int item);

int consome (tbuffer* buffer, int meuid);

void finalizabuffer (tbuffer* buffer);