<?php
class SaurusFileMigration extends Migration {
  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->description = t('Node images');

    $query = Database::getConnection('default', 'legacy_saurus')
            ->select('obj_file','f')
            ->fields('f', array('objekt_id', 'fullpath', 'relative_path', 'filename', 'mimetype', 'size', 'lastmodified', 'is_deleted',
                    'profile_id', 'notes', 'kirjeldus'));
    $query->orderBy('objekt_id', 'DESC');
    $query->range(0,30);

    $this->source = new MigrateSourceSQL($query);
    $this->destination = new MigrateDestinationFile('file');
    $this->map = new MigrateSQLMap($this->machineName,
        array(
          'objekt_id' => array(
            'type' => 'int',
            'not null' => TRUE,
            'description' => 'File ID.',
            'alias' => 'f',
          )
        ),
        MigrateDestinationFile::getKeySchema()
    );

    $this->addFieldMapping('value', 'relative_path');
    $this->addFieldMapping('destination_file', 'relative_path');
    $this->addFieldMapping('source_dir')
         ->defaultValue('./sites/default/files/');
    $this->addFieldMapping('file_replace')
         ->defaultValue('MigrateFile::FILE_EXISTS_REUSE');

    $this->addUnmigratedDestinations(array(
      'destination_dir',
      'fid',
      'path',
#      'file_replace',
      'preserve_files',
#      'source_dir',
      'timestamp',
      'uid',
      'urlencode',
    ));

  }
}
