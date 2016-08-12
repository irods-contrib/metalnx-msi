mlxCreateVocabulary {
   msicreate_vocabulary("/msiZone/home/rods", "demoResc", "test.vocab", "rods");
   msiPhyPathReg("/msiZone/home/rods/test.vocab", "demoResc", "/etc/irods/vocabularies/msiZone/home/rods/test.vocab", "null", *Status);
}

mlxRemoveVocabulary {
   msiremove_vocabulary(*Path, *Resc);
}

mlxRemoveVocabMetadata {
   msiremove_vocab_metadata(*Path, *Resc, *AttrName);
}

mlxAddVocabMetadata {
   msiadd_vocab_metadata(*Path, *Resc, *AttrName, *AttrUnit, *AttrType);
}