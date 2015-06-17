var Ug = function($) {

  var autoScrolling = false;

  /* window querying */

  function viewportWidth() {
    if ('innerWidth' in window) {
      return window.innerWidth;
    } else if ('documentElement' in document) {
      return document.documentElement.clientWidth;
    } else {
      return document.body.clientWidth;
    }
  }

  function wideView(width) {
    return (typeof width === 'undefined' ? $(window).width() : width) > 800;
  }

  /* text manipulation */

  function squashedTextContent($element) {
    return $element.text().replace(/\s+/g, ' ');
  }

  function hyphenize(text) {
    return text.replace(/\s/g, '-');
  }

  /* menu and navigation - wide window */

  function scrollTo($section) {
    var targetPosition = $section.offset().top;
    autoScrolling = true;
    $('html, body').stop().animate(
      { 'scrollTop': targetPosition },
      'fast',
      function() {
        selectMenuItemFor($section);
        autoScrolling = false;
      }
    );
  }

  function $createAutolinkTo($header) {
    var text = squashedTextContent($header);
    var id = $header.closest('.js-ug-headed-section').prop('id');
    return $('<a class="js-ug-internal-link" href="#' + id + '">' + text + '</a>');
  }

  function contractMenu() {
    $('nav ul ul').hide();

  }

  function expandMenuItem($menuListItem) {
    $menuListItem.parent().show();
    $menuListItem.children('ul').show();
  }

  function selectMenuItemFor($section) {
    var $link = $('nav [href="#' + $section.prop('id') + '"]');
    if ($link.size() === 0) {
      selectMenuItemFor($section.parent().closest('.js-ug-headed-section'));
    } else {
      var $menuItem = $link.closest('li');
      contractMenu();
      expandMenuItem($menuItem);
      $menuItem.children('a').focus();
    }
  }

  function createMenu() {
    var $nav = $('nav');
    var $ul = $('<ul></ul>').appendTo($nav);
    $('h2').each(function(index, header) {
      var $header = $(header);
      var $listItem = $('<li></li>').append($createAutolinkTo($header)).appendTo($ul);
      var $subHeaders = $header.closest('.js-ug-headed-section').find('h3');
      if ($subHeaders.size() !== 0) {
        var $subList = $('<ul></ul>');
        $subHeaders.each(function(index, subHeader) {
          var $subHeader = $(subHeader);
          $('<li></li>').
            append($createAutolinkTo($subHeader)).
            appendTo($subList);
        });
        $subList.appendTo($listItem);
      }
    });
    configureForSize();
    contractMenu();
  }

  function enableScrollToTop() {
    $('.js-ug-scroll-to-top').click(contractMenu);
  }

  function activateMenuItemsOnScroll() {
    $('.js-ug-headed-section').waypoint(function(direction) {
      if (!autoScrolling) {
        selectMenuItemFor($(this));
      }
    });
  }

  function expandMenuForHash() {
    var hash = window.location.hash.substring(1);
    if (hash) {
      selectMenuItemFor($('#' + hash));
    }
  }

  /* toggling sections - for narrow window */

  function toggleMajorSection(event) {
    event.preventDefault();
    event.stopPropagation();
    $(this)
      .closest('.js-ug-headed-section-major')
      .find('.js-ug-headed-section-major-body')
      .toggle();
  }

  /* internal links to headed sections */

  function configureAutolinks() {
    $('.js-ug-internal-link').click(function(event) {
      var $section = $($(this).attr('href'));
      $section.closest('.js-ug-headed-section-major').show();
      scrollTo($section);
    });
  }

  /* set things up depending on the viewport width */

  function configureForSize() {
    var width = viewportWidth();
    var w = Math.max(width * 0.25, 200);
    if (wideView(width)) {
      $('#ug-left-sidebar, nav').width(w);
      $('#ug-left-sidebar').show();
      $('#ug-main').css({ 'margin-left': w + 20 + 'px' });
      $('.js-ug-headed-section-major-body').show();
      $('.js-ug-headed-section-major-title').each(function() {
        $(this).removeClass('clickable-title').off('click', toggleMajorSection);
      });
    } else {
      $('#ug-left-sidebar, nav').width(width);
      $('#ug-main').css({ 'margin-left': '0' });
      $('.js-ug-headed-section-major-body').hide();
      $('.js-ug-headed-section-major-title:not(.clickable-title)').each(function() {
        $(this).on('click', toggleMajorSection).addClass('clickable-title');
      });
    }
  }

  /* initialize */

  function initialize() {
    createMenu();
    activateMenuItemsOnScroll();
    configureAutolinks();
    contractMenu();
    expandMenuForHash();
    $(window).resize(configureForSize);
  }

  return {
    initialize: initialize
  }

}(jQuery);

$(document).ready(Ug.initialize);
