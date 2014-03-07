<?php

/*
 * Migrate saurus users
 */
class SaurusUserMigration extends SaurusMigration {

  //if locked=1, return status=0 (disabled)
  protected function locked_to_status($value) {
    return !$value;
  }

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->description = t('Migrates Saurus users');
    $this->destination = new MigrateDestinationUser();

    $this->map = new MigrateSQLMap($this->machineName,
        array('user_id' => array(
              'type' => 'int',
              'not null' => TRUE,
              'description' => 'User ID.'
              )
        ),
        MigrateDestinationUser::getKeySchema()
    );

    $this->addFieldMapping('name', 'username')
         ->dedupe('users', 'name');
    $this->addFieldMapping('mail', 'email');
    $this->addFieldMapping('created', 'created_date')
    ->callbacks(array($this,'check_date'));
    $this->addFieldMapping('access', 'last_access_time')
    ->callbacks(array($this,'check_date'));
    $this->addFieldMapping('status', 'is_locked')
    ->callbacks(array($this,'locked_to_status'));
    $this->addFieldMapping('init', 'email');

    /**
     * TODO: Add role imports. For now, assign normal rid.
     */
    $this->addFieldMapping('roles')
         ->defaultValue(DRUPAL_AUTHENTICATED_RID);
    $this->addFieldMapping('timezone')
         ->defaultValue('Europe/Tallinn');


    $this->addUnmigratedDestinations(array(
#      'access',
      'password',
      'data',
      'is_new',
      'language',
      'login',
      'picture',
      'role_names',
      'signature',
      'signature_format',
      'theme',
#      'timezone',
    ));
  }
}
