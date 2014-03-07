<?php
/*
 * Migrate Saurus articles tagged as news to drupal as node:news
 */
class ExampleNewsMigration extends SaurusMigration {

  public $arguments = array();

  /**
   * [$source_table_obj - default 'objekt' table in saurus db]
   * Stores all objects inside (eg. files, articles, links, menu etc.)
   * @var string
   */
  private $source_table_obj = 'objekt';
  /**
   * [$source_table_obj_obj - default 'objekt_objekt' table in saurus db]
   * Stores (all) object relations in a hiearchical form.
   * @var string
   */
  private $source_table_obj_obj = 'objekt_objekt';
  /**
   * [$source_table_art - default 'obj_artikkel' table in saurus db]
   * Stores article contents inside
   * @var string
   */
  private $source_table_art = 'obj_artikkel';

  /**
   * [$lang - default '0']
   * langcodes:
   * Estonian - 0
   * English - 1
   * Russian - 2
   * Since Saurus is an Estonian CMS, assume we want stonian content by default.
   * PS! Use 'migrate_ui' module to see how it affects "Total rows" column.
   * @var string
   */
  protected $lang = '0';
  protected $nid_lang;

  public function __construct($arguments) {
    $this->arguments = $arguments;

    /**
     * Lang filter for saurus db select
     */
    if(isset($this->arguments['language'])){
      $this->lang = $this->arguments['language'];
    }

    /**
     * Lang filter for drupal node insert/update.
     */
    switch ($this->lang) {
      case '0':
      $this->nid_lang = 'et';
      break;
      case '1':
      $this->nid_lang = 'en';
      break;
      case '2':
      $this->nid_lang = 'ru';
      break;

      default:
      $this->nid_lang = 'et';
      break;
    }

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

    parent::__construct($arguments);

    $this->description = t('Migrate Saurus articles tagged as news to drupal as node:news');
    $this->destination = new MigrateDestinationNode('news');

    $this->source = new MigrateSourceSQL($this->query());

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
    ->defaultValue($this->nid_lang);

    $this->addFieldMapping('field_subject:source_type')
    ->defaultValue(TRUE);

    $this->addFieldMapping('created', 'aeg')
    ->callbacks(array($this,'check_date'));

    $this->addFieldMapping('changed', 'aeg')
    ->callbacks(array($this,'check_date'));

  }

  /**
   * [query - Build SQL query to fetch data from Saurus CMS DB]
   * @return QueryConditionInterface
   */
  private function query(){
    $query = Database::getConnection('default', 'legacy_saurus')
    ->select($this->source_table_obj,'o')
    ->fields('o', array('objekt_id', 'pealkiri', 'tyyp_id', 'author', 'on_avaldatud', 'keel', 'aeg', 'created_user_id'));
    $query->innerJoin($this->source_table_art, 'a', 'a.objekt_id = o.objekt_id');
    $query->innerJoin($this->source_table_obj_obj, 'oo', 'oo.objekt_id = a.objekt_id');
    $query->fields('a', array('sisu', 'lyhi'));
    $query->fields('oo', array('parent_id'));
    $query->condition('tyyp_id', '2', '=');
    $query->condition('keel', $this->lang, '='); //Default language, usually Estonian
    $query->condition('parent_id', '11118', '='); // Only select articles with news TID
    $query->orderBy('parent_id', 'ASC');

    return $query;
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

