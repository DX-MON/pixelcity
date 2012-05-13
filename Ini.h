int IniInt(const char *entry);
void IniIntSet(const char *entry, int val);
float IniFloat(const char *entry);
void IniFloatSet(const char *entry, float val);
char *IniString(const char *entry);
void IniStringSet(const char *entry, char *val);
void IniVectorSet(const char *entry, GLvector v);
GLvector IniVector(const char *entry);
