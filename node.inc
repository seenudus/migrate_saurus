<?php
/*
 * Migrate articles as node:article
 */
class SaurusNodeMigration extends SaurusMigration {

  public $arguments = array();

  public function __construct($arguments) {
    $this->arguments = $arguments;

    /**
     * Add user migration dependency
     */
    if (!empty($arguments['user_migration'])) {
      $user_migration = $arguments['user_migration'];
      $this->dependencies[] = $user_migration;
    }
    /**
     * Add menu term migration dependency
     */
    if (!empty($arguments['menu_migration'])) {
      $menu_migration = $arguments['menu_migration'];
      $this->dependencies[] = $menu_migration;
    }

    if (!empty($arguments['lang'])) {
      switch ($arguments['lang']) {
        case '0':
        $this->lang = 'et';
        break;
        case '1':
        $this->lang = 'en';
        break;
        case '2':
        $this->lang = 'ru';
        break;
        default:
        $this->lang = 'et';
        break;
      }
    }
    parent::__construct($arguments);

    $this->description = t('Migrates Saurus articles as nodes');
    $this->destination = new MigrateDestinationNode('article');

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'objekt_id' => array(
          'type' => 'int',
          'not null' => TRUE,
          'description' => 'Object ID.',
          'alias' => 'o',
          )
        ),
      MigrateDestinationNode::getKeySchema()
      );

    $this->addFieldMapping('title', 'pealkiri')
    ->description(t('Maps "pealkiri" field in source table to node title'));

    if (!empty($this->arguments['user_migration'])) {
      $this->addFieldMapping('uid', 'created_user_id')
      ->description(t('Maps "uid" field in source table to node author field.
        PS! Requires user migration to be ran firsthand!'))
      ->sourceMigration($this->arguments['user_migration'])
      ->defaultValue(0);
    } else {
      $this->addFieldMapping('uid')
      ->defaultValue(0);
    }




    if (!empty($this->arguments['menu_migration'])) {
      $this->addFieldMapping('field_subject', 'parent_id')
      ->sourceMigration($this->arguments['menu_migration'])
      ->defaultValue(0);
    }

    $this->addFieldMapping('body', 'sisu');
    $this->addFieldMapping('body:summary', 'lyhi');
    $this->addFieldMapping('body:format')
    ->defaultValue('full_html');
    $this->addFieldMapping('language')
    ->defaultValue($this->lang);

    $this->addFieldMapping('field_subject:source_type')
    ->defaultValue(TRUE);

    $this->addFieldMapping('created', 'aeg')
    ->callbacks(array($this,'check_date'));
  }

  /**
   * [prepareRow - Removes saurus inline code]
   * @param  $row object
   * @return null
   */
  public function prepareRow($row) {
    $pattern = '/##saurus\d{0,3}code##/';
    $row->sisu = preg_replace($pattern, '', $row->sisu) . "\r\n<br>";
  }

  /**
   * [complete - Make this node as the base for i18n]
   * @param  $entity
   * @param  $row
   * @return null
   */
  public function complete($entity, $row) {
    // $source_node = node_load($entity->tnid);
    // // Syncronize translation fields
    // i18n_sync_node_update($source_node);
  }
}

