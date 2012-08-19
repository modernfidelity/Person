<?php

/**
 * @file
 * person type editing UI.
 */

/**
 * UI controller.
 */
class PersonTypeUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
		$items[$this->path]['description'] = 'Manage person entity types, including adding
		and removing fields and the display of fields.';
    return $items;
  }
}

/**
 * Generates the person type editing form.
 */
function person_type_form($form, &$form_state, $person_type, $op = 'edit') {

  if ($op == 'clone') {
    $person_type->label .= ' (cloned)';
    $person_type->type = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $person_type->label,
    '#description' => t('The human-readable name of this person type.'),
    '#required' => TRUE,
    '#size' => 30,
  );
  // Machine-readable type name.
  $form['type'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($person_type->type) ? $person_type->type : '',
    '#maxlength' => 32,
//    '#disabled' => $person_type->isLocked() && $op != 'clone',
    '#machine_name' => array(
      'exists' => 'person_get_types',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this person type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['data']['#tree'] = TRUE;
  $form['data']['sample_data'] = array(
    '#type' => 'checkbox',
    '#title' => t('An interesting person switch'),
    '#default_value' => !empty($person_type->data['sample_data']),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save person type'),
    '#weight' => 40,
  );

  //Locking not supported yet
  /*if (!$person_type->isLocked() && $op != 'add') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete person type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('person_type_form_submit_delete')
    );
  }*/
  return $form;
}

/**
 * Form API submit callback for the type form.
 */
function person_type_form_submit(&$form, &$form_state) {
  $person_type = entity_ui_form_submit_build_entity($form, $form_state);
  $person_type->save();
  $form_state['redirect'] = 'admin/structure/person_types';
}

/**
 * Form API submit callback for the delete button.
 */
function person_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/person_types/manage/' . $form_state['person_type']->type . '/delete';
}
