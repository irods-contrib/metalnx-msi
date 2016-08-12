mlxCreateVocabulary {
   #msicreate_vocabulary("/msiZone/home/rods", "demoResc", "test.vocab", "rods");
   msicreate_vocabulary(*path, *resc, *vocab_name, *vocab_author);
   msiPhyPathReg("/msiZone/home/rods/test.vocab", "demoResc", "/etc/irods/vocabularies/msiZone/home/rods/test.vocab", "null", *Status);
}