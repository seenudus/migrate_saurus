<?php
/*
 * Migrate articles as node:article
 */
class ExampleNodeMigration extends SaurusNodeMigration {

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

  /**
   * @param array $arguments
   */
  public function __construct($arguments) {
    parent::__construct($arguments);

    $this->source = new MigrateSourceSQL($this->query());
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
    $query->orderBy('parent_id', 'ASC');

    return $query;
  }
}
