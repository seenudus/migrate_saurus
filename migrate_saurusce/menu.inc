<?php
/*
 * Migrate Saurus menu taxonomy to Drupal
 */
class ExampleMenuTermMigration extends SaurusMenuTermMigration {


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
      $source_table_obj = 'objekt';
      $source_table_obj_obj = 'objekt_objekt';
      $source_table_obj_rub = 'obj_rubriik';

      $query = Database::getConnection('default', 'legacy_saurus')
              ->select($source_table_obj,'o')
              ->fields('o', array('objekt_id', 'friendly_url', 'count', 'pealkiri', 'tyyp_id', 'author', 'on_avaldatud', 'keel', 'aeg', 'created_user_id', 'is_hided_in_menu'));
      $query->innerJoin($source_table_obj_obj, 'o_rel', 'o_rel.objekt_id = o.objekt_id');
      $query->leftJoin($source_table_obj, 'parents', 'parents.objekt_id = o_rel.parent_id');
      $query->addField('o_rel', 'parent_id');
      $query->addField('parents', 'pealkiri', 'parent_name');
      $query->condition('o.tyyp_id', '1','=');
      $query->condition('o.keel', '0', '=');
      $query->orderBy('parent_id', 'ASC');
      return $query;
  }
}
