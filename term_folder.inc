<?php
class SaurusFolderTermMigration extends Migration {
  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->description = t('Migrate Saurus Folder Taxonomy terms to Drupal');

    drupal_set_message($this->machineName);

    $this->source = new MigrateSourceSQL($this->query());
    
    #TODO make this dynamic variable, allowing automatic vocabulary creation
    $this->destination =
      new MigrateDestinationTerm('folder');

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'objekt_id' => array(
          'type' => 'int',
          'not null' => TRUE,
          'unsigned' => TRUE,
          'description' => 'Folder ID.',
          'alias' => 'o',
        ),
      ),
        MigrateDestinationTerm::getKeySchema()
      );

    $source_fields = array(
      'new_pid' => t('new pid'),
    );

    $this->addFieldMapping('name', 'pealkiri');
    $this->addFieldMapping('description')
         ->defaultValue($this->machineName);

    $this->addFieldMapping('parent', 'new_pid');
    $this->addFieldMapping('format')
         ->defaultValue('filtered_html');
    $this->addFieldMapping('weight')
         ->defaultValue(0);
    $this->addFieldMapping('path','friendly_url');
  }

  protected function query(){
      $source_table_obj = 'objekt';
      $source_table_obj_obj = 'objekt_objekt';
      $source_table_obj_f = 'obj_folder';

      $query = Database::getConnection('default', 'legacy_saurus')
              ->select($source_table_obj,'o')
              ->fields('o', array('objekt_id', 'friendly_url', 'count', 'tyyp_id', 'author', 'on_avaldatud', 'keel', 'aeg', 'created_user_id', 'is_hided_in_menu'));
      $query->innerJoin($source_table_obj_f, 'f', 'f.objekt_id = o.objekt_id');
      $query->leftJoin($source_table_obj_obj, 'o_rel', 'o_rel.objekt_id = f.objekt_id');
      $query->leftJoin($source_table_obj, 'parents', 'parents.objekt_id = o_rel.parent_id');
      $query->addField('o_rel', 'parent_id');
      $query->addField('f','relative_path', 'pealkiri');
      $query->addField('parents', 'pealkiri', 'parent_name');
      $query->condition('f.objekt_id', '', '!=');
      $query->orderBy('parent_id', 'ASC');
      return $query;
  }

  public function saurus_get_new_id($class, $id) {
  $query = db_select('migrate_map_' . $class, 'm')
    ->fields('m', array('destid1'))
    ->condition('m.sourceid1', $id, '=');
  $result = $query->execute();
  $output = '';
  foreach ($result as $row) {
    $output = $row->destid1;
  }
  return $output;
  }

  public function prepareRow($current_row) {
    //For some reason, $this->sourceMigration($this->machineName) does not map parent id''s to new id's, probably bug?'
    //Set the correct pid for the child by hand.
    $current_row->new_pid = $this->saurus_get_new_id(strtolower('SaurusFolderTaxonomy'), $current_row->parent_id);
    return TRUE;
  }
}
