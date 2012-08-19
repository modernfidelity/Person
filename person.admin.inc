<?php

/**
 * @file
 * person editing UI.
 *
 * We make very little use of the EntityAPI interface for this - preferring instead to use
 * views. That offers more flexibility to change a UI that will, more often than not,
 * be end-user facing.
 */

/**
 * UI controller.
 */
class PersonUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults. Main reason for doing this is that
   * parent class hook_menu() is optimized for entity type administration.
   */
  public function hook_menu() {
    
    $items = array();
    $id_count = count(explode('/', $this->path));
    $wildcard = isset($this->entityInfo['admin ui']['menu wildcard']) ? $this->entityInfo['admin ui']['menu wildcard'] : '%' . $this->entityType;

    $items[$this->path] = array(
      'title' => 'persons',
      'description' => 'Add edit and update persons.',
      'page callback' => 'system_admin_menu_block_page',
      'access arguments' => array('access administration pages'),
      'file path' => drupal_get_path('module', 'system'),
      'file' => 'system.admin.inc',
    );
    
    // Change the overview menu type for the list of persons.
    $items[$this->path]['type'] = MENU_LOCAL_TASK;
    
    // Change the add page menu to multiple types of entities
    $items[$this->path . '/add'] = array(
      'title' => 'Add a person',
      'description' => 'Add a new person',
      'page callback'  => 'person_add_page',
      'access callback'  => 'person_access',
      'access arguments' => array('edit'),
      'type' => MENU_NORMAL_ITEM,
      'weight' => 20,
      'file' => 'person.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module'])

    );
    
    // Add menu items to add each different type of entity.
    foreach (person_get_types() as $type) {
      $items[$this->path . '/add/' . $type->type] = array(
        'title' => 'Add ' . $type->label,
        'page callback' => 'person_form_wrapper',
        'page arguments' => array(person_create(array('type' => $type->type))),
        'access callback' => 'person_access',
        'access arguments' => array('edit', 'edit ' . $type->type),
        'file' => 'person.admin.inc',
        'file path' => drupal_get_path('module', $this->entityInfo['module'])
      );
    }

    // Loading and editing person entities
    $items[$this->path . '/person/' . $wildcard] = array(
      'page callback' => 'person_form_wrapper',
      'page arguments' => array($id_count + 1),
      'access callback' => 'person_access',
      'access arguments' => array('edit', $id_count + 1),
      'weight' => 0,
      'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
      'file' => 'person.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module'])
    );
    $items[$this->path . '/person/' . $wildcard . '/edit'] = array(
      'title' => 'Edit',
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => -10,
      'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
    );
    
    $items[$this->path . '/person/' . $wildcard . '/delete'] = array(
      'title' => 'Delete',
      'page callback' => 'person_delete_form_wrapper',
      'page arguments' => array($id_count + 1),
      'access callback' => 'person_access',
      'access arguments' => array('edit', $id_count + 1),
      'type' => MENU_LOCAL_TASK,
      'context' => MENU_CONTEXT_INLINE,
      'weight' => 10,
      'file' => 'person.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module'])
    );
    
    // Menu item for viewing persons
    $items['person/' . $wildcard] = array(
      //'title' => 'Title',
      'title callback' => 'person_page_title',
      'title arguments' => array(1),
      'page callback' => 'person_page_view',
      'page arguments' => array(1),
      'access callback' => 'person_access',
      'access arguments' => array('view', 1),
      'type' => MENU_CALLBACK,
    );
    return $items;
  }
  
  
  /**
   * Create the markup for the add person Entities page within the class
   * so it can easily be extended/overriden.
   */ 
  public function addPage() {
    $item = menu_get_item();
    $content = system_admin_menu_block($item);

    if (count($content) == 1) {
      $item = array_shift($content);
      drupal_goto($item['href']);
    }    
        
    return theme('person_add_list', array('content' => $content));
  }
  
}


/**
 * Form callback wrapper: create or edit a person.
 *
 * @param $person
 *   The person object being edited by this form.
 *
 * @see person_edit_form()
 */
function person_form_wrapper($person) {
  // Add the breadcrumb for the form's location.
  person_set_breadcrumb();
  return drupal_get_form('person_edit_form', $person);
}


/**
 * Form callback wrapper: delete a person.
 *
 * @param $person
 *   The person object being edited by this form.
 *
 * @see person_edit_form()
 */
function person_delete_form_wrapper($person) {
  // Add the breadcrumb for the form's location.
  //person_set_breadcrumb();
  return drupal_get_form('person_delete_form', $person);
}


/**
 * Form callback: create or edit a person.
 *
 * @param $person
 *   The person object to edit or for a create form an empty person object
 *     with only a person type defined.
 */
function person_edit_form($form, &$form_state, $person) {
  // Add the default field elements.
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('person Name'),
    '#default_value' => isset($person->name) ? $person->name : '',
    '#maxlength' => 255,
    '#required' => TRUE,
    '#weight' => -5,
  );
  
  $form['data']['#tree'] = TRUE;
  $form['data']['sample_data'] = array(
    '#type' => 'checkbox',
    '#title' => t('An interesting person switch'),
    '#default_value' => isset($person->data['sample_data']) ? $person->data['sample_data'] : 1,
  );

  // Add the field related form elements.
  $form_state['person'] = $person;
  field_attach_form('person', $person, $form, $form_state);

  $form['actions'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('form-actions')),
    '#weight' => 400,
  );

  // We add the form's #submit array to this button along with the actual submit
  // handler to preserve any submit handlers added by a form callback_wrapper.
  $submit = array();

  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save person'),
    '#submit' => $submit + array('person_edit_form_submit'),
  );
  
  if (!empty($person->name)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete person'),
      '#suffix' => l(t('Cancel'), 'admin/content/persons'),
      '#submit' => $submit + array('person_form_submit_delete'),
      '#weight' => 45,
    );
  }

  // We append the validate handler to #validate in case a form callback_wrapper
  // is used to add validate handlers earlier.
  $form['#validate'][] = 'person_edit_form_validate';
  return $form;
}


/**
 * Form API validate callback for the person form
 */
function person_edit_form_validate(&$form, &$form_state) {
  $person = $form_state['person'];
  
  // Notify field widgets to validate their data.
  field_attach_form_validate('person', $person, $form, $form_state);
}


/**
 * Form API submit callback for the person form.
 * 
 * @todo remove hard-coded link
 */
function person_edit_form_submit(&$form, &$form_state) {
  
  $person = entity_ui_controller('person')->entityFormSubmitBuildEntity($form, $form_state);
  // Save the person and go back to the list of persons
  
  // Add in created and changed times.
  if ($person->is_new = isset($person->is_new) ? $person->is_new : 0){
    $person->created = time();
  }

  $person->changed = time();
  
  $person->save();
  $form_state['redirect'] = 'admin/content/persons';
}

/**
 * Form API submit callback for the delete button.
 * 
 * @todo Remove hard-coded path
 */
function person_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/content/persons/person/' . $form_state['person']->person_id . '/delete';
}


/**
 * Form callback: confirmation form for deleting a person.
 *
 * @param $person
 *   The person to delete
 *
 * @see confirm_form()
 */
function person_delete_form($form, &$form_state, $person) {
  $form_state['person'] = $person;

  $form['#submit'][] = 'person_delete_form_submit';

  $form = confirm_form($form,
    t('Are you sure you want to delete person %name?', array('%name' => $person->name)),
    'admin/content/persons/person',
    '<p>' . t('This action cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );
  
  return $form;
}

/**
 * Submit callback for person_delete_form
 */
function person_delete_form_submit($form, &$form_state) {
  $person = $form_state['person'];

  person_delete($person);

  drupal_set_message(t('The person %name has been deleted.', array('%name' => $person->name)));
  watchdog('person', 'Deleted person %name.', array('%name' => $person->name));

  $form_state['redirect'] = 'admin/content/persons';
}



/**
 * Page to add person Entities.
 *
 * @todo Pass this through a proper theme function
 */
function person_add_page() {
  $controller = entity_ui_controller('person');
  return $controller->addPage();
}


/**
 * Displays the list of available person types for person creation.
 *
 * @ingroup themeable
 */
function theme_person_add_list($variables) {
  $content = $variables['content'];
  $output = '';
  if ($content) {
    $output = '<dl class="person-type-list">';
    foreach ($content as $item) {
      $output .= '<dt>' . l($item['title'], $item['href']) . '</dt>';
      $output .= '<dd>' . filter_xss_admin($item['description']) . '</dd>';
    }
    $output .= '</dl>';
  }
  else {
    if (user_access('administer person types')) {
      $output = '<p>' . t('person Entities cannot be added because you have not created any person types yet. Go to the <a href="@create-person-type">person type creation page</a> to add a new person type.', array('@create-person-type' => url('admin/structure/person_types/add'))) . '</p>';
    }
    else {
      $output = '<p>' . t('No person types have been created yet for you to use.') . '</p>';
    }
  }

  return $output;
}





/**
 * Sets the breadcrumb for administrative person pages.
 */
function person_set_breadcrumb() {
  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Administration'), 'admin'),
    l(t('Content'), 'admin/content'),
    l(t('persons'), 'admin/content/persons'),
  );

  drupal_set_breadcrumb($breadcrumb);
}



