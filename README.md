> ###Saurus to Drupal migration
This is a base Saurus CMS migration, from which you can extend your site specific migrations.

#### Users:
    User passwords can't be transferred, since they are encrypted 1 way.
      Workarounds/fixes:
        * "Reset password" method that sends temporary login link to user's email (works by default)
        * You can create a module that does form_alter that runs on login form and adds auth logic from old saurus site, allowing user to log in. This is up to you to implement.

#### Misc:
    Database is in utf8 encoding, but the data itself is in legacy latin1 encoding.
    This creates problems with umlauts, so we need to convert to real utf8.
      Workarounds/fixes:
        * Dump the db as latin1 and reimport using utf8. Example:
            mysqldump -u username -p --skip-set-charset --default-character-set=latin1 -Q db_name > db_dump.sql
            mysql --force -u username -p --default-character-set=utf8 db_name < db_dump.sql
