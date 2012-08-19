<?php

/**
 * @file
 * Contains a Views field handler to take care of displaying edit links
 * as fields
 */


class person_handler_edit_link_field extends person_handler_link_field {
  function construct() {
    parent::construct();
    $this->additional_fields['type'] = 'type';
  }


  function render($values) {
    $type = $values->{$this->aliases['type']};
    
    //Creating a dummy person to check access against
    $dummy_person = (object) array('type' => $type);
    if (!person_access('edit', $dummy_person)) {
      return;
    }
    
    $text = !empty($this->options['text']) ? $this->options['text'] : t('edit');
    $person_id = $values->{$this->aliases['person_id']};
    
    return l($text, 'admin/content/persons/person/' . $person_id . '/edit');
  }
}
