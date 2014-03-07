<?php
/*
 * Migrate Saurus users to Drupal
 */
class ExampleUserMigration extends SaurusUserMigration {


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
            ->select('users','u')
            ->fields('u', array('user_id', 'email', 'username', 'password', 'firstname', 'lastname', 'created_date', 'last_access_time',
                    'is_locked', 'address'))
            ->condition('username', '','!=');
    return $query;
  }
}
