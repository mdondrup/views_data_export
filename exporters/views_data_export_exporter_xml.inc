<?php

require_once 'views_data_export_exporter.inc';
require_once 'ViewsDataExportExporterUserConfigurationInterface.inc';

/**
 * Webform exporter for creating XML files.
 */
class ViewsDataExportExporterXML extends ViewsDataExportExporter implements ViewsDataExportExporterUserConfigurationInterface
{

  /**
   * Constructor for views_data_export_exporter classes.
   *
   * @param $options
   *   The array of export options as provided by the user-interface.
   */
  function __construct($options) {
//    $this->options = $options;

    // Ensure defaults are set.
    $definition = $this->option_definition();
    foreach ($definition as $option_name => $properties) {
      if (!isset($options[$option_name])) {
        $options[$option_name] = $properties['default'];
      }
    }

    $options['no_entity_encode'] = array_filter($options['no_entity_encode']);
    $options['cdata_wrapper'] = array_filter($options['cdata_wrapper']);
    $this->options = $options;

    parent::__construct($options);
  }

  /**
   * Tell the world whether we support Hide If Empty views option
   */
  function supports_hide_if_empty() {
    return TRUE;
  }

  /**
   * Set options fields and default values.
   *
   * @return
   * An array of options information.
   */
  function option_definition() {
    $definition = array();

    $definition['transform'] = array(
      'default' => TRUE,
      'translatable' => FALSE,
    );
    $definition['transform_type'] = array(
      'default' => 'dash',
      'translatable' => FALSE,
    );
    $definition['root_node'] = array(
      'default' => 'root',
      'translatable' => FALSE,
    );
    $definition['item_node'] = array(
      'default' => 'item',
      'translatable' => FALSE,
    );
    $definition['no_entity_encode'] = array(
      'default' => array(),
      'translatable' => FALSE,
    );
    $definition['cdata_wrapper'] = array(
      'default' => array(),
      'translatable' => FALSE,
    );

    return $definition;
  }

  /**
   * Options form mini callback.
   *
   * @param $form
   * Form array to add additional fields to.
   * @param $form_state
   * State of the form.
   * @return
   * None.
   */
  function options_form(&$form, &$form_state, $field_labels) {

    $form['transform'] = array(
      '#type' => 'checkbox',
      '#title' => t('Transform spaces'),
      '#default_value' => $this->options['transform'],
      '#description' => t('Transform spaces to valid XML in field labels (spaces create invalid XML markup). Note that invalid XML tag characters will always be converted.'),
    );
    $form['transform_type'] = array(
      '#type' => 'select',
      '#title' => t('Transform type'),
      '#default_value' => $this->options['transform_type'],
      '#options' => array(
        'dash' => t('Dash'),
        'underline' => t('Underline'),
        'camel' => t('camelCase'),
        'pascal' => t('PascalCase'),
      ),
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array(
        'edit-style-options-transform' => array(TRUE),
      ),
    );
    $form['root_node'] = array(
      '#type' => 'textfield',
      '#title' => t('Root node'),
      '#default_value' => $this->options['root_node'],
      '#description' => t('The XML tag for the root node.'),
    );
    $form['item_node'] = array(
      '#type' => 'textfield',
      '#title' => t('Item node'),
      '#default_value' => $this->options['item_node'],
      '#description' => t('The XML tag for an item node.'),
    );

    if (!empty($field_labels)) {
      if (empty($options['no_entity_encode'])) {
        $options['no_entity_encode'] = array();
      }

      $form['no_entity_encode'] = array(
        '#type' => 'checkboxes',
        '#title' => t('Disable encoding of XML entities for these fields'),
        '#options' => $field_labels,
        '#default_value' =>$this->options['no_entity_encode'],
        '#description' => t('If checked field contents will be outputted '.
          '<em>without encoding</em> of XML entities. This is '.
          'useful when when used in conjunction with a field '.
          'formatter that outputs properly formatted and '.
          'encoded XML data.'),
      );

      if (empty($options['cdata_wrapper'])) {
        $options['cdata_wrapper'] = array();
      }

      $form['cdata_wrapper'] = array(
        '#type' => 'checkboxes',
        '#title' => t('Field values to wrap using CDATA'),
        '#options' => $field_labels,
        '#default_value' => $this->options['cdata_wrapper'],
        '#description' => t('If checked the fields content will be wrapped using the CDATA tag.'),
      );
    }
  }

  /**
   * Perform any necessary changes to the form values prior to storage.
   * There is no need for this function to actually store the data.
   *
   * @param $form
   * @param $form_state
   */
  function options_submit(&$form, &$form_state) {
    if (isset($form_state['values']['style_options']['no_entity_encode'])) {
      // Remove any options values set to 0
      $form_state['values']['style_options']['no_entity_encode'] = array_filter($form_state['values']['style_options']['no_entity_encode']);
    }
    if (isset($form_state['values']['style_options']['cdata_wrapper'])) {
      // Remove any options values set to 0
      $form_state['values']['style_options']['cdata_wrapper'] = array_filter($form_state['values']['style_options']['cdata_wrapper']);
    }
  }

  /**
   * Add a single row to the export file.
   *
   * @param $file_handle
   *   A PHP file handle to the export file.
   * @param array $data
   *   An array of formatted data for this row. One cell per item.
   * @param int $row_count
   *   The current number of rows in the export file.
   * @param $field_titles
   */
  function add_row(&$file_handle, $data, $row_count, $field_titles) {

    fwrite($file_handle, '<' . $this->options['item_node'] . '>');
    $this->array_to_xml($data, $file_handle, null, $field_titles);
    fwrite($file_handle, '</' . $this->options['item_node'] . '>');

  }

  function array_to_xml($array, &$file_handle, $open_key, $field_titles) {
    $key_open = false;
    foreach($array as $key => $value) {

      $clean_tag = $this->_clean_tag_from_key($key, $field_titles);

      // It's a parent node - open it and recurse to render the children.
      if (is_array($value)) {
        // Open a parent key.
        fwrite($file_handle, '<' . $this->clean_xml_tag($field_titles[$key]) . '>');
        $key_open = true;
        // Recursive call, to render children.
        $this->array_to_xml($value, $file_handle, $key, $field_titles);

      // It's a child/leaf node - render it.
      } else {


        // Don't encode <> if it contains valid XML ready for export.
        // (Or if it's about to be wrapped in CDATA tags!)
        if (in_array($key, $this->options['no_entity_encode'], TRUE) || in_array($key, $this->options['cdata_wrapper'], TRUE)) {
          $value_to_write = $value;
        } else {
          $value_to_write = htmlspecialchars($value);
        }

        // Wrap target elements in CDATA tags.
        if (in_array($key, $this->options['cdata_wrapper'], TRUE)) {
          $value_to_write = '<![CDATA[' . $value_to_write . ']]>';
        }

        // Write element to the file.
        fwrite($file_handle, '<' . $clean_tag . '>' . $value_to_write . '</' . $clean_tag . '>');
      }
      // Close the open parent key.
      if ($key_open) {
        fwrite($file_handle, '</' . $clean_tag . '>');
        $key_open = false;
      }
    }
  }

  function _clean_tag_from_key($key, $field_titles) {

    // Get the actual field title for the key, then check it's clean.
    // If we can't get one, we'll clean the one we have.
    if (isset($field_titles[$key])) {
      $key = $field_titles[$key];
    }

    // Make sure key/attribute names are valid.
    if (isset($this->options['transform']) && $this->options['transform'] == true) {
      // Perform the transforms.
      switch ($this->options['transform_type']) {
        case 'dash': $key = str_replace(' ', '-', $key);
          break;

        case 'underline': $key = str_replace(' ', '_', $key);
          break;

        case 'camelCase':
          $key = ucwords($key);
          $key = lcfirst($key);
          $key = str_replace(' ', '', $key);
          break;

        case 'PascalCase':
          $key = ucwords($key);
          $key = str_replace(' ', '', $key);
          break;
      }
    }

    $clean_tag = $this->clean_xml_tag($key);

    return $clean_tag;
  }


  /**
   * Provide headers to the page when an export file is being downloaded.
   *
   * @param $filename
   *   The name of the file being downloaded. e.g. export.xls.
   */
  function get_headers($filename) {
    $headers = array();
    $headers['Content-Type'] = 'application/force-download';
    $headers['Pragma'] = 'public';
    $headers['Cache-Control'] = 'max-age=0';
    return $headers;
  }

  /**
   * Write the start of the export file.
   *
   * @param $file_handle
   *   A PHP file handle to the export file.
   */
  function bof(&$file_handle) {
    fwrite($file_handle, '<?xml version="1.0" encoding="UTF-8" ?>' . PHP_EOL . '<' . $this->options['root_node'] . '>');
  }

  /**
   * Write the end of the export file.
   *
   * @param $file_handle
   *   A PHP file handle to the export file.
   */
  function eof(&$file_handle, $row_count, $col_count) {
    fwrite($file_handle, '</' . $this->options['root_node'] . '>' );
  }

  /**
   * Allow final processing of the results.
   *
   * @param $results
   *   An array of result data, including:
   *   - file_name: The full file path of the generated file.
   *   - row_count: The final number of rows in the generated file.
   */
  function post_process(&$results) {
  }

  /**
   * Validate the options.
   */
  function options_validate(&$form, &$form_state) {
  }
}
