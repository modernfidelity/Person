<?php

/**
 * This field handler aggregates operations that can be done on a person
 * under a single field providing a more flexible way to present them in a view
 */
class person_handler_person_operations_field extends views_handler_field {
  function construct() {
    parent::construct();

    $this->additional_fields['person_id'] = 'person_id';
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {

    $links = menu_contextual_links('person', 'admin/content/persons/person', array($this->get_value($values, 'person_id')));
    
    if (!empty($links)) {
      return theme('links', array('links' => $links, 'attributes' => array('class' => array('links', 'inline', 'operations'))));
    }
  }
}
