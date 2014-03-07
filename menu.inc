<?php
class SaurusMenuTermMigration extends SaurusMigration {

  /**
   * [createMenuVocabulary - Creates taxonomy vocabulary for menu term migration]
   * @param  string $vocabulary_name         [Vocabulary name]
   * @param  string $vocabulary_machine_name [Vocabulary machine name]
   * @return string $vocabulary_machine_name
   */
  private static function createMenuVocabulary($vocabulary_name = 'Saurus migrate menu', $vocabulary_machine_name = 'saurus_migrate_menu'){
    if(!taxonomy_vocabulary_machine_name_load($vocabulary_machine_name)){
      $vocabulary = new stdClass();
      $vocabulary->name = $vocabulary_name;
      $vocabulary->machine_name = $vocabulary_machine_name;
      taxonomy_vocabulary_save($vocabulary);
    }
    return $vocabulary_machine_name;
  }

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->description = t('Migrate Saurus Menu Taxonomy terms to Drupal');

    $this->destination =
    new MigrateDestinationTerm($this->createMenuVocabulary());

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'objekt_id' => array(
          'type' => 'int',
          'not null' => TRUE,
          'unsigned' => TRUE,
          'description' => 'Menu ID.',
          'alias' => 'o',
          ),
        ),
      MigrateDestinationTerm::getKeySchema()
      );

    $this->addFieldMapping('name', 'pealkiri');
    $this->addFieldMapping('description')
    ->defaultValue($this->machineName);
    $this->addUnmigratedDestinations(array('parent_name'));
    $this->addFieldMapping('parent', 'parent_id')
    ->sourceMigration($this->machineName);
    $this->addFieldMapping('format')
    ->defaultValue('filtered_html');
    $this->addFieldMapping('weight')
    ->defaultValue(0);
    $this->addFieldMapping('path','friendly_url');
  }
}
